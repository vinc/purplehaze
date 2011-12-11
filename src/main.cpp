/* PurpleHaze 2.0.0
 * Copyright (C) 2007-2011  Vincent Ollivier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

using namespace std;

string prompt() {
    string cmd;
    cout << "> ";
    cin >> cmd;
    return cmd;
}

int main() {
    cout << "Purple Haze " << VERSION << endl;
    cout << endl;

    // Parse commands from CLI
    string fen(DEFAULT_FEN);
    string cmd = prompt();
    while (cmd != "quit" && cmd != "exit") {
        if (cmd == "xboard") { // Xboard protocol mode
            Xboard xboard;
            xboard.loop();
            return 0;
        }
        else if (cmd == "help") {
            int comm_width = 30;
            int desc_width = 50;
            cout << left;
            cout << setw(comm_width) << "help"
                 << setw(desc_width)
                 << "display this screen"
                 << endl;
            cout << setw(comm_width) << "version"
                 << setw(desc_width)
                 << "display the version"
                 << endl;
            cout << setw(comm_width) << "setboard <fen>"
                 << setw(desc_width)
                 << "set the board according to <fen> position"
                 << endl;
            cout << setw(comm_width) << "perft"
                 << setw(desc_width)
                 << "count all nodes at each depth from the starting position"
                 << endl;
            cout << setw(comm_width) << "divide"
                 << setw(desc_width)
                 << "count all nodes from each moves at the starting position"
                 << endl;
            cout << setw(comm_width) << "testsuite <epd> [<time>]"
                 << setw(desc_width)
                 << "load an <epd> test suite and search each position"
                 << endl
                 << setw(comm_width) << ""
                 << setw(desc_width)
                 << "for <time> seconds"
                 << endl;
            cout << setw(comm_width) << "xboard"
                 << "start XBoard protocol mode"
                 << endl;
            cout << setw(comm_width) << "quit"
                 << "quit Purple Haze"
                 << endl;
        }
        else if (cmd == "version") {
            cout << VERSION << endl;
        }
        else if (cmd == "setboard") { // Get FEN
            getline(cin, fen);
            fen.erase(0, 1); // Remove the first whitespace
        }
        else if (cmd == "perft") {
            Game game;
            game.init(fen);
            for (int i = 1; ; ++i) {
                clock_t start = clock();
                unsigned int perft_result = game.perft(i);
                double perft_time = double(clock() - start) / CLOCKS_PER_SEC;
                cout << "Perft(" << i << ") = " << perft_result;
                cout << " (" << perft_time << " secs, ";
                cout << perft_result / perft_time << " nps)" << endl;
            }
        }
        else if (cmd == "divide") {
            int depth = 0;
            cin >> depth;
            cout << endl;
            Game game;
            game.init(fen);
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
                    cout << move << " " << cnt << endl;
                }
                game.undo_move(move);
            }
            cout << endl;
            cout << "Moves: " << moves_count << endl;
            cout << "Positions: " << nodes_count << endl;
        }
        else if (cmd == "testsuite") { // Load EPD test suite
            string filename;
            cin >> filename;
            ifstream epdfile;
            epdfile.open(filename.c_str());
            if (!epdfile.is_open()) {
                cerr << "Cannot open '" << filename;
                cerr << "': No such file or directory" << endl;
            }

            // Get time per move (optional)
            string seconds;
            getline(cin, seconds);
            int time = 10;
            if (seconds != "") {
                istringstream iss(seconds);
                iss >> time;
            }
            
            cout << "Loading '" << filename << "', "; 
            cout << time << "s per move" << endl; // In seconds
            
            // Load game protocol
            Protocol proto;
            proto.set_output_thinking(false);
            proto.set_time(1, time);
            
            // Read positions in file
            unsigned int res = 0;
            unsigned int i = 0;
            while (epdfile.good()) {
                proto.new_game();
                string line;
                getline(epdfile, line);
                // TODO: add am (avoid move)
                size_t fensep = line.find(" bm ");
                size_t bmsep = line.find(";");
                if (fensep == string::npos || bmsep == string::npos) continue;
                
                // Load position in game
                fen = line.substr(0, fensep);
                cout << "Loading position #" << i + 1 << " '" << fen << "' ";
                proto.set_board(fen);
                
                // Search best move and test it
                string best_moves = line.substr(fensep + 4, bmsep - fensep - 4);
                cout << "bm " << best_moves;
                string move = proto.search_move(true);
                cout << " => " << move;
                istringstream iss(best_moves);
                bool is_found = false;
                do {
                    string best_move;
                    iss >> best_move;
                    if (best_move == move) {
                        is_found = true;
                        break;
                    }
                } while (iss);

                if (is_found) {
                    cout << " OK" << endl;
                    ++res;
                }
                else {
                    cout << " KO" << endl;
                }
                ++i;
            }
            cout << "Result: " << res << "/" << i << endl;
            epdfile.close();
        }
        cmd = prompt();
    }        
    return 0;
}
