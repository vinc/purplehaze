/* Copyright (C) 2007-2012 Vincent Ollivier
 *
 * Purple Haze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Purple Haze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream>
#include <iterator>
#include <csignal>
#include <sstream>
#include <string>
#include <vector>

#include "xboard.h"

typedef std::vector<std::string> Tokens;

static Tokens tokenize(const std::string line)
{
    std::istringstream iss(line);
    Tokens tokens;
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter<Tokens>(tokens));
    assert(tokens.size() > 0);
    return tokens;
}

static std::string read_line(Log &log)
{
    std::string line;
    getline(std::cin, line);
    if (line.length() == 0) {
        line = "quit";
    } else if (line.substr(0, 4) != "ping") {
        log.to(Log::FILE) << Log::IN << line << std::endl;
    }
    return line;
}

static bool is_move(const std::string move)
{
    return 4 <= move.size() && move.size() <= 5 &&
           'a' <= move[0] && move[0] <= 'h' &&
           '1' <= move[1] && move[1] <= '8' &&
           'a' <= move[2] && move[2] <= 'h' &&
           '1' <= move[3] && move[3] <= '8';
}

void Xboard::think()
{
    log.to(Log::FILE) << Log::DEBUG
                      << "FEN '" << game.fen() << "'" << std::endl;
    if (get_verbosity() > 1) {
        log.to(Log::FILE) << game.board << std::endl;
    }

    const std::string move = search_move();
    std::string output = "";
    if (move == "LOST") {
        output = "result ";
        if (game.positions.current().side() == WHITE) {
            output += "0-1 {Black mates}";
        } else {
            output += "1-0 {White mates}";
        }
    } else if (move == "DRAW") {
        output = "result 1/2-1/2";
    } else if (!force_mode) {
        play_move(move);

        const float elapsed = game.time.elapsed() / 100.0f;
        const float allocated = game.time.allocated() / 100.0f;
        log.to(Log::FILE) << Log::DEBUG
                          << elapsed << " of " << allocated
                          << " second" << (allocated != 1.0f ? "s" : "")
                          << " used to play" << std::endl;

        if (get_verbosity() > 1) {
            log << std::endl;
            game.print_tt_stats();
        }
        output = "move " + move;
    }
    log.to(Log::BOTH) << Log::OUT << output << std::endl;
}

void Xboard::loop()
{
    signal(SIGINT, SIG_IGN);
    log << std::endl; // Acknowledge Xboard mode

    const std::time_t t = std::time(NULL);
    std::string datetime = std::ctime(&t);
    datetime.erase(datetime.end() - 1); // Remove trailing new line character
    log.to(Log::FILE) << Log::DEBUG << "[" << datetime << "] "
                      << "Purple Haze " << VERSION << std::endl
                      << Log::IN << "xboard" << std::endl;

    std::string cmd;
    while ((cmd = read_line(log)) != "quit") {
        const Tokens args = tokenize(cmd);
        if (args[0] == "protover") {
            assert(args.size() == 2);
            const int version = std::stoi(args[1]);
            if (version == 2) {
                for (const std::string (&feature)[2] : XBOARD_FEATURES) {
                    std::string title = feature[0];
                    std::string value = feature[1];
                    if (value != "0" && value != "1") {
                        // Non boolean values must be quoted
                        value = '"' + value + '"';
                    }
                    std::string out = "feature " + title + "=" + value;
                    log.to(Log::BOTH) << Log::OUT << out << std::endl;
                }
            }
        } else if (args[0] == "accepted" || args[0] == "rejected") {
            assert(args.size() == 2);
            const std::string feature = args[1];

            // Given the current feature list, there is nothing to be done
            // when a feature gets accepted or rejected.
        } else if (args[0] == "new") {
            assert(args.size() == 1);
            new_game();
            set_board(DEFAULT_FEN);
            force_mode = false;
        } else if (args[0] == "setboard") {
            new_game();
            set_board(cmd.erase(0, std::string("setboard ").length()));
            force_mode = false;
        } else if (args[0] == "go") {
            assert(args.size() == 1);
            force_mode = false;
            if (thinker.joinable()) {
                thinker.join();
            }
            thinker = std::thread(&Xboard::think, this);
        } else if (args[0] == "?") {
            assert(args.size() == 1);
            game.time.abort();
        } else if (args[0] == "force") {
            assert(args.size() == 1);
            force_mode = true;
        } else if (args[0] == "ping") {
            if (thinker.joinable()) {
                thinker.join(); // Wait before replying to 'ping'
            }
            log.to(Log::FILE) << Log::IN << cmd << std::endl;
            assert(args.size() == 2);
            const int n = std::stoi(args[1]);
            log.to(Log::BOTH) << Log::OUT << "pong " << n << std::endl;
        } else if (args[0] == "level") {
            assert(args.size() == 4);
            int moves = std::stoi(args[1]); // Number of moves

            // TODO "level 0 m 0" means play the entire game in 'm' minutes,
            // but the current time management don't support it.
            if (moves == 0) {
                moves = 60;
            }

            // Time interval in minutes or minutes:seconds
            int time;
            size_t sep = args[2].find(":");
            if (sep == std::string::npos) {
                const int minutes = std::stoi(args[2]);
                time = minutes * 60;
            } else {
                const int minutes = std::stoi(args[2].substr(0, sep));
                const int seconds = std::stoi(args[2].substr(sep + 1));
                time = minutes * 60 + seconds;
            }

            // Control character
            const int control = std::stoi(args[3]);

            if (control == 0) {
                set_time(moves, time);
            } else { // Not in Xboard protocol
                // TODO If not zero, control is a time increment,
                // but currently this time is not directly used
                set_time(moves, time);
            }
        } else if (args[0] == "time") {
            assert(args.size() == 2);
            const int time = std::stoi(args[1]);
            set_remaining(time);
        } else if (args[0] == "otim") {
            assert(args.size() == 2);
        } else if (args[0] == "sd") {
            assert(args.size() == 2);
            const int d = std::stoi(args[1]);
            set_depth(d);
        } else if (args[0] == "undo") {
            assert(args.size() == 1);
            undo_move();
        } else if (args[0] == "remove") {
            assert(args.size() == 1);
            undo_move();
            undo_move();
        } else if (args[0] == "post") {
            assert(args.size() == 1);
            set_output_thinking(true);
        } else if (args[0] == "nopost") {
            assert(args.size() == 1);
            set_output_thinking(false);
        } else if (args[0] == "hard") {
            assert(args.size() == 1);
        } else if (is_move(args[0]) && !parse_move(args[0]).is_null()) {
            assert(args.size() == 1);
            std::string move = args[0];
            log.to(Log::FILE) << Log::DEBUG
                              << "move '" << move << "' successfully parsed"
                              << std::endl;

            if (!play_move(move)) {
                log.to(Log::BOTH) << Log::OUT
                                  << "Illegal move: " << move << std::endl;
                continue;
            }
            if (!force_mode) {
                if (thinker.joinable()) {
                    thinker.join();
                }
                thinker = std::thread(&Xboard::think, this);
            }
        } else if (args[0] == "verbose") { // Debug mode
            assert(args.size() == 1);
            verbosity = 2;
        } else {
            log.to(Log::FILE) << Log::DEBUG
                              << "unrecognized command '" << cmd << "'"
                              << std::endl;
        }
    }
    if (thinker.joinable()) {
        thinker.join();
    }
}
