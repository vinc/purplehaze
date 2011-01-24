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
#include <iostream>
#include <string>

#include "xboard.h"

using namespace std;

Xboard::Xboard() {
   log.open("game.log", ios::app); 
   force_mode = true;
}

void Xboard::think() {
    //cout << game.board << endl;
    string move = search_move();
    string output = "";
    if (move == "LOST") {
	output = "result ";
	if (game.current_node().get_turn_color() == WHITE) {
	    output += "0-1 {Black mates}";
	}
	else {
	    output += "1-0 {White mates}";
	}
    }	
    else if (move == "DRAW") {
	output = "result 1/2-1/2";
    }
    else if (!force_mode) {
	play_move(move);
	output = "move " + move;
    }
    cout << output << endl;
    log << output << endl;
}

void Xboard::loop() {
    cout << endl; // Acknowledge Xboard mode
    log << "DEBUG: PurpleHaze starting" << endl;
    cout << "feature setboard=1" << endl;
    cout << "feature sigint=0" << endl;
    //cout << "feature ping=1" << endl;
    cout << "feature done=1" << endl;

    string cmd;
    cin >> cmd;
    while (cmd != "quit") {
	log << ">" << cmd << endl; 
	if (cmd == "new") {
	    new_game();
	    set_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	    force_mode = false;
	}
	else if (cmd == "go") {
	    force_mode = false;
	    think();
	}
	else if (cmd == "force") {
	    force_mode = true;
	}
	else if (cmd == "ping") {
	    int n;
	    cin >> n;
	    cout << "pong " << n << endl;
	}
	else if (cmd == "sd") {
	    int d = 0;
	    cin >> d;
	    set_depth(d);
	}
	else if (cmd == "undo") {
	    undo_move();
	}
	else if (cmd == "remove") {
	    undo_move();
	    undo_move();
	}
	else if (cmd == "hard") {

	}
	else if (cmd == "level") {

	}
	else if (4 <= cmd.size() && cmd.size() <= 5) {
	    if (!parse_move(cmd).is_null()) { // TODO This need improvement
		log << "DEBUG: move parsed: " << cmd << endl;
		play_move(cmd);
		think();
	    }
	}
	else {
	    log << "DEBUG: ignoring: " << cmd << endl;
	}
	cin >> cmd;
    }
    log << ">" << cmd << endl; 
    log << "DEBUG: exiting" << endl; 
}
