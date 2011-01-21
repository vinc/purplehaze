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

#include "game.h"

using namespace std;

Game game;
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
    // Parse commands from CLI
    string cmd = prompt();
    while (cmd != "quit" && cmd != "exit") {
	if (cmd == "xboard") { // Xboard protocol mode
	    //xboard_loop();
	    return 0;
	}
	else if (cmd == "version") {
	    cout << version << endl;
	}
	else if (cmd == "perft") {
	    game.init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	    for (int i = 0; ; ++i) {
		clock_t start = clock();
		int perft_result = game.perft(i);
		double perft_time = double(clock() - start) / CLOCKS_PER_SEC;
		cout << "Perft(" << i << ") = " << perft_result;
		cout << " (" << perft_time << " secs, ";
		cout << perft_result / perft_time << " nps)" << endl;
	    }
	}
	cmd = prompt();
    }	
    return 0;
}
