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

#include <cassert>
#include <ctime>
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "common.h"
#include "game.h"
#include "xboard.h"

static const std::string OPTIONS[][2] = {
    {
        "help",
        "display this screen"
    },
    {
        "version",
        "display the version"
    },
    {
        "setboard",
        "set the board according to <fen> position"
    },
    {
        "perft",
        "count all nodes at each depth from the starting position"
    },
    {
        "divide",
        "count all nodes from each moves at the starting position"
    },
    {
        "testsuite <epd> [<time>]",
        "search each position of <epd> for <time> seconds"
    },
    {
        "perftsuite <epd>",
        "compare perft results to each position of <epd>"
    },
    {
        "xboard",
        "start XBoard protocole mode"
    },
    {
        "quit",
        "exit Purple Haze"
    }
};

static const std::string FLAGS[][2] = {
    { "-c",      "use colored output" },
    { "-l FILE", "write log to FILE" },
    { "-m NUM",  "set material table size to NUM megabytes" },
    { "-t NUM",  "set transposition table size to NUM megabytes" }
};

void print_usage() {
    std::cout << "Usage: purplehaze [OPTION]" << std::endl;
    std::cout << std::left;
    int option_width = 15;
    for (const std::string (&option)[2] : FLAGS) {
        std::string name = option[0];
        std::string usage = option[1];
        std::cout << "  "
                  << std::setw(option_width) << name
                  << std::setw(80 - 2 - option_width) << usage
                  << std::endl;
    }
}

static std::string prompt()
{
    std::cout << "> ";
    std::string cmd;
    std::cin >> cmd;
    if (cmd == "") {
        cmd = "quit";
    }
    return cmd;
}

int main(int argc, char *argv[])
{
    std::string logfile = "";
    bool option_color = false;
    int tt_size = TT_SIZE;
    int mt_size = MT_SIZE;
    signed char opt;
    while ((opt = getopt(argc, argv, "chl:m:t:")) != EOF) {
        switch (opt) {
        case 'c':
            option_color = true;
            break;
        case 'h':
            print_usage();
            return 0;
        case 'l':
            logfile = optarg;
            break;
        case 'm':
            mt_size = std::stoi(optarg) << 20;
            break;
        case 't':
            tt_size = std::stoi(optarg) << 20;
            break;
        }
    }
    std::cout << "Purple Haze " << VERSION << std::endl;
    std::cout << std::endl;

    // Parse commands from CLI
    std::string init_fen(DEFAULT_FEN);
    std::string cmd;
    while ((cmd = prompt()) != "quit") {
        if (cmd == "xboard") { // Xboard protocol mode
            std::cin.ignore(); // Discards end of line
            Xboard xboard(tt_size, mt_size);
            if (!logfile.empty()) {
                xboard.debug(logfile);
            }
            xboard.loop();
            return 0;
        } else if (cmd == "help") {
            std::cout << std::left;
            int comm_width = 30;
            int desc_width = 50;
            for (const std::string (&option)[2] : OPTIONS) {
                std::string name = option[0];
                std::string usage = option[1];
                std::cout << std::setw(comm_width) << name
                          << std::setw(desc_width) << usage
                          << std::endl;
            }
        } else if (cmd == "version") {
            std::cout << VERSION << std::endl;
        } else if (cmd == "setboard") { // Get FEN
            getline(std::cin, init_fen);
            init_fen.erase(0, 1); // Remove the first whitespace
        } else if (cmd == "perft") {
            Game game(tt_size, mt_size);
            game.init(init_fen);
            for (unsigned int i = 1; ; ++i) {
                clock_t start = clock();
                unsigned long long int perft_result = game.perft(i);
                long double clocks = clock() - start;
                long double perft_time = clocks / CLOCKS_PER_SEC;
                std::cout << "Perft(" << i << ") = " << perft_result;
                std::cout << " (" << perft_time << " secs, ";
                std::cout << perft_result / perft_time << " nps)" << std::endl;
            }
        } else if (cmd == "divide") {
            int depth = 0;
            std::cin >> depth;
            std::cout << std::endl;
            Game game(tt_size, mt_size);
            game.init(init_fen);
            Color c = game.current_position().side();
            unsigned int nodes_count = 0;
            unsigned int moves_count = 0;
            Moves moves(game.board, game.pieces, game.current_position(),
                        game.search_moves);
            Move move;
            while (!(move = moves.next()).is_null()) {
                game.make_move(move);
                if (!game.is_check(c)) {
                    const unsigned int n = game.perft(depth - 1);
                    nodes_count += n;
                    ++moves_count;
                    std::cout << move << " " << n << std::endl;
                }
                game.undo_move(move);
            }
            std::cout << std::endl;
            std::cout << "Moves: " << moves_count << std::endl;
            std::cout << "Positions: " << nodes_count << std::endl;
        } else if (cmd == "testsuite") { // Load EPD test suite
            std::string filename;
            std::cin >> filename;
            std::string args;
            getline(std::cin, args);

            // Check if filename exists
            std::ifstream epdfile;
            epdfile.open(filename.c_str());
            if (!epdfile.is_open()) {
                std::cerr << "Cannot open '" << filename << "': "
                          << "No such file or directory" << std::endl;
                continue;
            }

            // Get time per move (optional)
            int time = 10;
            if (args != "") {
                std::istringstream iss(args);
                if (!(iss >> time)) {
                    args.erase(0, 1); // Remove whitespace
                    std::cerr << "Invalid argument '" << args << "'"
                              << std::endl;
                    continue;
                }
            }

            std::cout << "Loading '" << filename << "', ";
            std::cout << time << "s per move" << std::endl; // In seconds

            // Load game protocol
            Protocol proto(tt_size, mt_size);
            proto.set_output_thinking(false);
            proto.set_time(1, time);

            // Read positions in file
            unsigned int res = 0;
            unsigned int i = 0;
            std::string line;
            while (getline(epdfile, line)) {
                proto.new_game();
                // TODO: add am (avoid move)
                size_t fensep = line.find(" bm ");
                size_t bmsep = line.find(";");
                size_t not_found = std::string::npos;
                if (fensep == not_found || bmsep == not_found) {
                    continue;
                }

                // Load position in game
                init_fen = line.substr(0, fensep);
                std::cout << "FEN #" << i + 1 << " '" << init_fen << "' ";
                proto.set_board(init_fen);

                // Search best move and test it
                std::string best_moves = line.substr(fensep + 4,
                                                     bmsep - fensep - 4);
                std::cout << "bm " << best_moves;
                std::string move = proto.search_move(true);
                std::cout << " => " << move;
                std::istringstream iss(best_moves);
                bool is_found = false;
                do {
                    std::string best_move;
                    iss >> best_move;
                    if (best_move == move) {
                        is_found = true;
                        break;
                    }
                } while (iss);

                if (is_found) {
                    if (option_color) {
                        std::cout << color_green;
                    }
                    std::cout << " OK";
                    ++res;
                } else {
                    if (option_color) {
                        std::cout << color_red;
                    }
                    std::cout << " KO";
                }
                if (option_color) {
                    std::cout << color_reset;
                }
                std::cout << std::endl;
                ++i;
            }
            std::cout << "Result: " << res << "/" << i << std::endl;
            epdfile.close();
        } else if (cmd == "perftsuite") { // Load perft test suite
            std::string filename;
            std::cin >> filename;
            std::ifstream epdfile;
            epdfile.open(filename.c_str());
            if (!epdfile.is_open()) {
                std::cerr << "Cannot open '" << filename << "': "
                          << "No such file or directory" << std::endl;
                continue;
            }
            const size_t npos = std::string::npos;
            std::string line;
            while (getline(epdfile, line)) {
                std::string fen;
                Game game(tt_size, mt_size);
                for (int i = 0; line.length() > 0; ++i) {

                    const size_t pos = line.find(" ;");
                    std::string sub = line.substr(0, pos);
                    line = (pos != npos ? line.substr(pos + 2, npos) : "");

                    if (i == 0) {
                        fen = sub;
                        game.init(fen);
                        std::cout << "FEN '" << fen << "': " << std::flush;
                    } else {
                        sub = sub.substr(3, npos); // Skip /^D\d /
                        unsigned int moves = game.perft(i);
                        unsigned int expected;
                        std::istringstream(sub) >> expected;
                        if (option_color) {
                            std::cout << (moves == expected ? color_green
                                                            : color_red);
                        }
                        std::cout << (moves == expected ? "." : "x")
                                  << (option_color ? color_reset : "")
                                  << std::flush;
                    }
                }
                std::cout << std::endl;
            }
            epdfile.close();
        } else {
            std::cerr << "Invalid command '" << cmd << "'" << std::endl;
        }
    }
    return 0;
}
