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
#include <string>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "common.h"
#include "game.h"
#include "xboard.h"

using namespace std;

string version = "PurpleHaze 2.0.0";

string prompt() {
    string cmd;
    cout << "> ";
    cin >> cmd;
    return cmd;
}

int main() {
    cout << version << endl;
    cout << endl;

    cout << "Piece: " << sizeof(Piece) << endl;
    cout << "Move: " << sizeof(Move) << endl;
    cout << "ExtendedMove: " << sizeof(ExtendedMove) << endl;
    cout << "Transposition: " << sizeof(Transposition) << endl;
    //cout << size_t << endl;
    cout << endl;

    // Parse commands from CLI
    string cmd = prompt();
    while (cmd != "quit" && cmd != "exit") {
	if (cmd == "xboard") { // Xboard protocol mode
	    Xboard xboard;
	    xboard.loop();
	    return 0;
	}
	else if (cmd == "version") {
	    cout << version << endl;
	}
	else if (cmd == "perft") {
	    Game game;
	    game.init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	    for (int i = 1; ; ++i) {
		clock_t start = clock();
		int perft_result = game.perft(i);
		double perft_time = double(clock() - start) / CLOCKS_PER_SEC;
		cout << "Perft(" << i << ") = " << perft_result;
		cout << " (" << perft_time << " secs, ";
		cout << perft_result / perft_time << " nps)" << endl;
	    }
	}
	else if (cmd == "setboard") {
	    // Get FEN
	    Game game;
	    string fen;
	    getline(cin, fen);
	    fen.erase(0, 1); // Remove the first whitespace
	    game.init(fen);
	}
	else if (cmd == "divide") {
	    Game game;
	    game.init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	    int depth = 0;
	    cin >> depth;
	    cout << endl;
	    Color c = game.current_node().get_turn_color();
	    int nodes_count = 0;
	    int moves_count = 0;
	    Moves moves = game.movegen();
	    moves.numeric_sort();
	    //for (moves.it = moves.begin(); moves.it != moves.end(); moves.it++) {
	    //	game.make_move(*moves.it);
	    for (int i = 0; i < moves.size(); ++i) {
	    	game.make_move(moves.at(i));
		if (!game.is_check(c)) {
		    int cnt = game.perft(depth - 1);
		    nodes_count += cnt;
		    moves_count++;
		    cout << moves.at(i) << " " << cnt << endl;
		}
		game.undo_move(moves.at(i));
	    }
	    cout << endl;
	    cout << "Moves: " << moves_count << endl;
	    cout << "Nodes: " << nodes_count << endl;

	}
	cmd = prompt();
    }	
    return 0;
}
