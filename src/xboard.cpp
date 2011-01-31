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
#include <sstream>
#include <string>
#include <signal.h>

#include "xboard.h"

using namespace std;

Xboard::Xboard() {
   force_mode = true;
}

void Xboard::think() {
    cout << game.board << endl;
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
    signal(SIGINT, SIG_IGN);
    cout << endl; // Acknowledge Xboard mode
    log.open("game.log", ios::app); 
    log << "DEBUG: PurpleHaze starting" << endl;

    string cmd;
    cin >> cmd;
    while (cmd != "quit") {
	log << ">" << cmd << endl; 
	if (cmd == "protover") {
	    int n;
	    cin >> n;
	    if (n == 2) {
		for (int i = 0; i < XBOARD_NB_FEATURES; ++i) {
		    cout << "feature " << XBOARD_FEATURES[i][0];
		    cout << "=" << XBOARD_FEATURES[i][1] << endl;
		    string reply, feature;
		    cin >> reply;
		    cin >> feature;
		    assert(feature == XBOARD_FEATURES[i][0]);
		    if (reply == "accepted") continue;
		    else if (reply == "rejected") assert(false); // FIXME
		    else assert(false); // FIXME
		}
	    }
	}
	else if (cmd == "new") {
	    new_game();
	    set_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	    force_mode = false;
	}
	else if (cmd == "setboard") {
	    string fen;
	    getline(cin, fen);
	    fen.erase(0, 1); // Remove the first whitespace
	    new_game();
	    set_board(fen);
	    force_mode = false;
	}
	else if (cmd == "go") {
	    force_mode = false;
	    cout << game.board << endl;
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
	else if (cmd == "level") {
	    // Number of moves
	    int moves;
	    cin >> moves;

	    // Time interval in minutes or minutes:seconds
	    int time;
	    string str_time;
	    cin >> str_time;
	    size_t sep = str_time.find(":");
	    if (sep == string::npos) {
		istringstream minutes(str_time);
		minutes >> time;
		time *= 60;
	    }
	    else {
		istringstream minutes(str_time.substr(0, sep));
		istringstream seconds(str_time.substr(sep + 1));
		seconds >> time;
		int tmp;
		minutes >> tmp;
		time += 60 * tmp;
	    }

	    // Control character
	    int control;
	    cin >> control;
	    if (control == 0) {
		set_time(moves, time);
		log << "> level " << moves << " " << time << endl;
	    }
	}
	else if (cmd == "time") {
	    int time = 0;
	    cin >> time;
	    set_remaining_time(time);
	}
	else if (cmd == "otim") {
	    int time = 0;
	    cin >> time;
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
	else if (4 <= cmd.size() && cmd.size() <= 5 &&
		 'a' <= cmd[0] && cmd[0] <= 'h' &&
		 '1' <= cmd[1] && cmd[1] <= '8' &&
		 'a' <= cmd[2] && cmd[2] <= 'h' &&
		 '1' <= cmd[3] && cmd[3] <= '8' &&
		 !parse_move(cmd).is_null()) {
	    log << "DEBUG: move parsed: " << cmd << endl;
	    if (!play_move(cmd)) {
		cout << "Illegal move: " << cmd << endl;
	    }
	    else think();
	}
	else {
	    log << "DEBUG: ignoring: " << cmd << endl;
	}
	cin >> cmd;
    }
    log << ">" << cmd << endl; 
    log << "DEBUG: exiting" << endl; 
    log.close();
}
