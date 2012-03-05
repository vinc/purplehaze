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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <time.h>

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
        "xboard",
        "start XBoard protocole mode"
    },
    {
        "quit",
        "exit Purple Haze"
    }
};

std::string prompt() {
    std::cout << "> ";
    std::string cmd;
    std::cin >> cmd;
    return cmd;
}

int main() {
    std::cout << "Purple Haze " << VERSION << std::endl;
    std::cout << std::endl;

    // Parse commands from CLI
    std::string init_fen(DEFAULT_FEN);
    for (std::string cmd = prompt(); cmd != "quit"; cmd = prompt()) {
        if (cmd == "xboard") { // Xboard protocol mode
            Xboard xboard;
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
            Game game;
            game.init(init_fen);
            for (unsigned int i = 1; ; ++i) {
                clock_t starting_time = clock();
                unsigned long long int perft_result = game.perft(i);
                long double elapsed = clock() - starting_time;
                long double perft_time = elapsed / CLOCKS_PER_SEC;
                std::cout << "Perft(" << i << ") = " << perft_result;
                std::cout << " (" << perft_time << " secs, ";
                std::cout << perft_result / perft_time << " nps)" << std::endl;
            }
        } else if (cmd == "divide") {
            int depth = 0;
            std::cin >> depth;
            std::cout << std::endl;
            Game game;
            game.init(init_fen);
            Color c = game.current_position().get_turn_color();
            unsigned int nodes_count = 0;
            unsigned int moves_count = 0;
            Moves moves(game.board, game.pieces, game.current_position(),
                        game.search_moves);
            Move move;
            while (!(move = moves.next()).is_null()) {
                    game.make_move(move);
                if (!game.is_check(c)) {
                    unsigned int cnt = game.perft(depth - 1);
                    nodes_count += cnt;
                    ++moves_count;
                    std::cout << move << " " << cnt << std::endl;
                }
                game.undo_move(move);
            }
            std::cout << std::endl;
            std::cout << "Moves: " << moves_count << std::endl;
            std::cout << "Positions: " << nodes_count << std::endl;
        } else if (cmd == "testsuite") { // Load EPD test suite
            std::string filename;
            std::cin >> filename;

            // Check if filename exists
            std::ifstream epdfile;
            epdfile.open(filename.c_str());
            if (!epdfile.is_open()) {
                std::cerr << "Cannot open '" << filename;
                std::cerr << "': No such file or directory" << std::endl;
            }

            // Get time per move (optional)
            std::string seconds;
            getline(std::cin, seconds);
            int time = 10;
            if (seconds != "") {
                std::istringstream iss(seconds);
                iss >> time;
            }

            std::cout << "Loading '" << filename << "', ";
            std::cout << time << "s per move" << std::endl; // In seconds

            // Load game protocol
            Protocol proto;
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
                if (fensep == not_found || bmsep == not_found) continue;

                // Load position in game
                init_fen = line.substr(0, fensep);
                std::cout << "Loading position #" << i + 1
                          << " '" << init_fen << "' ";
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
                    std::cout << " OK" << std::endl;
                    ++res;
                } else {
                    std::cout << " KO" << std::endl;
                }
                ++i;
            }
            std::cout << "Result: " << res << "/" << i << std::endl;
            epdfile.close();
        }
    }
    return 0;
}
