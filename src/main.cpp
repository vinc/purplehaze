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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "common.h"
#include "game.h"
#include "xboard.h"

using namespace std;

#ifndef VERSION
#define VERSION "2.0.1"
#endif

string prompt() {
    string cmd;
    cout << "> ";
    cin >> cmd;
    return cmd;
}

int main() {
    cout << "Purple Haze " << VERSION << endl;
    cout << endl;
    /*
    cout << "Hash: " << sizeof(Hash) << endl;
    cout << "Piece: " << sizeof(Piece) << endl;
    cout << "Move: " << sizeof(Move) << endl;
    cout << "ExtendedMove: " << sizeof(ExtendedMove) << endl;
    cout << "Transposition: " << sizeof(Transposition) << endl;
    cout << "Node: " << sizeof(Node) << endl;
    cout << "Moves: " << sizeof(Moves) << endl;
    cout << endl;

    cout << "Compiled with options:";
#ifdef NMP
    cout << " NMP";
#endif
#ifdef TT
    cout << " TT";
#endif
    cout << endl << endl;
    */

    string default_fen = 
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    // Parse commands from CLI
    string cmd = prompt();
    while (cmd != "quit" && cmd != "exit") {
	if (cmd == "xboard") { // Xboard protocol mode
	    Xboard xboard;
	    xboard.loop();
	    return 0;
	}
	else if (cmd == "version") {
	    cout << VERSION << endl;
	}
	else if (cmd == "setboard") {
	    // Get FEN
	    getline(cin, default_fen);
	    default_fen.erase(0, 1); // Remove the first whitespace
	}
	else if (cmd == "perft") {
	    Game game;
	    game.init(default_fen);
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
	    game.init(default_fen);
	    Color c = game.current_node().get_turn_color();
	    int nodes_count = 0;
	    int moves_count = 0;
	    Moves moves(game.board, game.pieces, game.current_node());    
	    Move move;
	    while (!(move = moves.next()).is_null()) {
	    	game.make_move(move);
		if (!game.is_check(c)) {
		    int cnt = game.perft(depth - 1);
		    nodes_count += cnt;
		    moves_count++;
		    cout << move << " " << cnt << endl;
		}
		game.undo_move(move);
	    }
	    cout << endl;
	    cout << "Moves: " << moves_count << endl;
	    cout << "Nodes: " << nodes_count << endl;
	}
	else if (cmd == "testsuite") {
	    // Get EPD test suite
	    string filename;
	    cin >> filename;
	    ifstream epdfile;
	    epdfile.open(filename);
	    if (!epdfile.is_open()) {
		cout << "Cannot open '" << filename;
		cout << "': No such file or directory" << endl;
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
	    int res = 0;
	    int i = 0;
	    while (epdfile.good()) {
		proto.new_game();
		string line;
		getline(epdfile, line);
		size_t fensep = line.find(" bm ");
		size_t bmsep = line.find(";");
		if (fensep == string::npos || bmsep == string::npos) continue;
		
		// Load position in game
		string fen = line.substr(0, fensep);
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
