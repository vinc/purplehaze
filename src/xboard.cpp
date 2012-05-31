/* Copyright (C) 2007-2011 Vincent Ollivier
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

#include <assert.h>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>

#include "xboard.h"

Xboard::Xboard()
{
    force_mode = true;
    debug_mode = false;
    verbosity = 1;
}

void Xboard::think()
{
    if (get_verbosity() > 1) std::cout << game.board << std::endl;

    std::string move = search_move();
    std::string output = "";
    if (move == "LOST") {
        output = "result ";
        if (game.current_position().side() == WHITE) {
            output += "0-1 {Black mates}";
        } else {
            output += "1-0 {White mates}";
        }
    } else if (move == "DRAW") {
        output = "result 1/2-1/2";
    } else if (!force_mode) {
        play_move(move);
        if (debug_mode) {
            log << std::endl << "  DEBUG: "
                << game.time.elapsed() / 100.0f
                << " of " << game.time.allocated() / 100.0f
                << " seconds used to play";
        }
        if (get_verbosity() > 1) {
            std::cout << std::endl;
            game.print_tt_stats();
        }
        output = "move " + move;
    }
    std::cout << output << std::endl;
    if (debug_mode) {
        log << std::endl << "< " << output;
    }
}

void Xboard::loop()
{
    signal(SIGINT, SIG_IGN);
    std::cout << std::endl; // Acknowledge Xboard mode

    std::string cmd;
    std::cin >> cmd;
    while (cmd != "quit") {
        if (debug_mode) {
            log << "> " << cmd;
        }
        if (cmd == "protover") {
            int n;
            std::cin >> n;
            if (debug_mode) {
                log << " " << n;
            }
            if (n == 2) {
                for (const std::string (&feature)[2] : XBOARD_FEATURES) {
                    std::string title = feature[0];
                    std::string value = feature[1];
                    if (value != "0" && value != "1") {
                        value = '"' + value + '"';
                    }
                    std::string out = "feature " + title + "=" + value;
                    std::cout << out << std::endl;
                    if (debug_mode) {
                        log << std::endl << "< " << out;
                    }
                }
            }
        } else if (cmd == "accepted") {
            std::string feature;
            std::cin >> feature;
            if (debug_mode) {
                log << std::endl << "> accepted " << feature;
            }
            // Given the current feature list, there is nothing to be done
            // when a feature gets accepted.
        } else if (cmd == "rejected") {
            std::string feature;
            std::cin >> feature;
            if (debug_mode) {
                log << std::endl << "> rejected " << feature;
            }
            // Given the current feature list, there is nothing to be done
            // when a feature gets rejected.
        } else if (cmd == "debug") {
            if (!debug_mode) {
                debug_mode = true;
                log.open("game.log", std::ios::app);
                log << "  DEBUG: Starting log" << std::endl;
            }
        } else if (cmd == "new") {
            new_game();
            set_board(DEFAULT_FEN);
            force_mode = false;
        } else if (cmd == "setboard") {
            std::string fen;
            getline(std::cin, fen);
            fen.erase(0, 1); // Remove the first whitespace
            new_game();
            set_board(fen);
            if (debug_mode) {
                log << " " << fen;
            }
            force_mode = false;
        } else if (cmd == "go") {
            force_mode = false;
            if (thinker.joinable()) {
                thinker.join();
            }
            thinker = std::thread(&Xboard::think, this);
        } else if (cmd == "?") {
            game.time.abort();
        } else if (cmd == "force") {
            force_mode = true;
        } else if (cmd == "ping") {
            int n;
            std::cin >> n;
            std::cout << "pong " << n << std::endl;
        } else if (cmd == "level") {
            // Number of moves
            int moves;
            std::cin >> moves;
            if (debug_mode) {
                log << " " << moves;
            }

            // TODO "level 0 m 0" means play the entire game in 'm' minutes,
            // but the current time management don't support it.
            if (moves == 0) moves = 60;

            // Time interval in minutes or minutes:seconds
            int time;
            std::string str_time;
            std::cin >> str_time;
            if (debug_mode) {
                log << " " << str_time;
            }
            size_t sep = str_time.find(":");
            if (sep == std::string::npos) {
                std::istringstream minutes(str_time);
                minutes >> time;
                time *= 60;
            } else {
                std::istringstream minutes(str_time.substr(0, sep));
                std::istringstream seconds(str_time.substr(sep + 1));
                seconds >> time;
                int tmp;
                minutes >> tmp;
                time += 60 * tmp;
            }

            // Control character
            int control;
            std::cin >> control;
            if (debug_mode) {
                log << " " << control;
            }
            if (control == 0) {
                set_time(moves, time);
            } else { // Not in Xboard protocol
                // TODO If not zero, control is a time increment,
                // but currently this time is not directly used
                set_time(moves, time);
            }
        } else if (cmd == "time") {
            int time = 0;
            std::cin >> time;
            if (debug_mode) {
                log << " " << time;
            }
            set_remaining(time);
        } else if (cmd == "otim") {
            int time = 0;
            std::cin >> time;
            if (debug_mode) {
                log << " " << time;
            }
        } else if (cmd == "sd") {
            int d = 0;
            std::cin >> d;
            if (debug_mode) {
                log << " " << d;
            }
            set_depth(d);
        } else if (cmd == "undo") {
            undo_move();
        } else if (cmd == "remove") {
            undo_move();
            undo_move();
        } else if (cmd == "post") {
            set_output_thinking(true);
        } else if (cmd == "nopost") {
            set_output_thinking(false);
        } else if (cmd == "hard") {

        } else if (4 <= cmd.size() && cmd.size() <= 5 &&
                 'a' <= cmd[0] && cmd[0] <= 'h' &&
                 '1' <= cmd[1] && cmd[1] <= '8' &&
                 'a' <= cmd[2] && cmd[2] <= 'h' &&
                 '1' <= cmd[3] && cmd[3] <= '8' &&
                 !parse_move(cmd).is_null()) {
            if (debug_mode) {
                log << std::endl << "  DEBUG: move '" << cmd << "' successfully parsed";
            }
            if (!play_move(cmd)) {
                std::cout << "Illegal move: " << cmd << std::endl;
                if (debug_mode) {
                    log << std::endl << "< Illegal move: " << cmd;
                }
            }
            if (!force_mode) {
                if (thinker.joinable()) {
                    thinker.join();
                }
                thinker = std::thread(&Xboard::think, this);
            }
        } else if (cmd == "verbose") { // Debug mode
            verbosity = 2;
        } else {
            if (debug_mode) {
                log << std::endl << "  DEBUG: unrecognized command:" << cmd;
            }
        }
        if (debug_mode) {
            log << std::endl;
        }
        std::cin >> cmd;
    }
    if (thinker.joinable()) {
        thinker.join();
    }
    if (debug_mode) {
        log << "> " << cmd << std::endl;
        log << "  DEBUG: Closing log" << std::endl;
        log.close();
    }
}
