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
#include <list>
#include <vector>
#include <stdio.h>
#include <time.h>

#include "game.h"

using namespace std;

int Game::perft(int depth) {
    int nodes_count = 0;
    Color c = current_node().get_turn_color();
    if (depth == 0) return 1;
    Moves moves = movegen();
    for (moves.it = moves.begin(); moves.it != moves.end(); moves.it++) {
	//cout << *moves.it << endl;
	make_move(*moves.it);
	if (!is_check(c)) {
	    nodes_count += perft(depth - 1);
	}
	undo_move(*moves.it);
    }
    return nodes_count;
}


int Game::search(int depth) {
    if (depth == 0) return eval();

    int score = -INF;
    int best_score = -INF;
    Color player = current_node().get_turn_color();
    Moves moves = movegen();
    Move best_move;
    bool legal_move_found = false;
    for (moves.it = moves.begin(); moves.it != moves.end(); moves.it++) {
	Move move = *moves.it;
	make_move(move);
	if (!is_check(player)) {
	    legal_move_found = true;
	    score = -search(depth - 1);
	    if (score > best_score) {
		best_score = score;
	    } 
	}
	undo_move(*moves.it);
    }
    if (!legal_move_found) {
	if (is_check(player)) return -INF + 100 - depth; // Checkmate
	else return 0; // Stalemate
    }
    return best_score;
}

Move Game::root(int max_depth) {
    int score = -INF;
    int best_score = -INF;
    Move best_move;
    Color player = current_node().get_turn_color();
    Moves moves = movegen();
    clock_t start = clock();
    for (moves.it = moves.begin(); moves.it != moves.end(); moves.it++) {
	Move move = *moves.it;
	make_move(move);
	if (!is_check(player)) {
	    score = -search(max_depth - 1);
	    cout << move << " " << score;
	    if (score > best_score) {
		best_score = score;
		best_move = move;
		cout << " <- new best move";
	    } 
	    cout << endl;
	}
	undo_move(*moves.it);
    }
    double perft_time = double(clock() - start) / CLOCKS_PER_SEC;
    cout << endl;
    cout << "Best move: " << best_move;
    cout << " (" << perft_time << " secs)" << endl;
    cout << endl;
    return best_move;
}
