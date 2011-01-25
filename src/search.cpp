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
#include <iomanip>

#include "game.h"

using namespace std;

#define WIDE 10

void print_search_legend() {
    cout << setw(4) << "ply";
    cout << setw(WIDE) << "score";
    cout << setw(WIDE) << "time";
    cout << setw(WIDE) << "nodes";
    cout << setw(WIDE) << " pv";
    cout << endl;
}

void print_search(int ply, int score, double time, int nodes) {
    cout << setw(4) << ply;
    cout << setw(WIDE) << score;
    cout << setw(WIDE) << setprecision(3) << time;
    if (nodes >= 1000000) {
	cout << setw(WIDE - 1) << setprecision(3) << nodes / 1000000.0 << "M";
    }
    else {
	cout << setw(WIDE) << nodes;
    }
    cout << endl;
}

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


int Game::search(int alpha, int beta, int depth) {
    if (depth == 0) return eval();

    int score; //= -INF;
    //int best_score = -INF;
    Color player = current_node().get_turn_color();
    Moves moves = movegen();
    Move best_move;

    Transposition* ptr_trans = tt.lookup(current_node().hash());
    if (ptr_trans && ptr_trans->get_depth() >= depth) { 
	int trans_score = ptr_trans->get_value();
	switch (ptr_trans->get_bound()) {
	    case EXACT:
		return trans_score;
		break;
	    case UPPER:
		//if (trans_score <= alpha) return trans_score;
		if (trans_score < beta) beta = trans_score;
		break;
	    case LOWER: 
		//if (trans_score >= beta) return trans_score;
		if (trans_score > alpha) alpha = trans_score;
		break;
	    default:
		break;
	}
	if (alpha >= beta) return trans_score;
    }
    if (ptr_trans) {
	Move bm = ptr_trans->get_best_move();
	if (!bm.is_null()) best_move = bm;
    }

    bool legal_move_found = false;
    moves.sort(best_move);
    for (moves.it = moves.begin(); moves.it != moves.end(); moves.it++) {
	Move move = *moves.it;
	make_move(move);

	if (is_check(player)) { // Illegal move
	    undo_move(*moves.it);
	    continue;
	}

	legal_move_found = true;
	
	score = -search(-beta, -alpha, depth - 1);
	
	undo_move(*moves.it);
	
	if (score >= beta) {
	    return beta;
	} 
	
	if (score > alpha) {
	    alpha = score;
	    best_move = move;
	} 
    }
    if (!legal_move_found) {
	if (is_check(player)) return -INF + 100 - depth; // Checkmate
	else return 0; // Stalemate
    }

    tt.save(current_node().hash(), alpha, alpha, beta, depth, best_move);

    return alpha;
}

Move Game::root(int max_depth) {
    Move best_move;
    Color player = current_node().get_turn_color();
    Moves moves = movegen();
    clock_t start = clock();

    print_search_legend();
    for (int ply = 1; ply < max_depth; ++ply) {
	int score;
	int alpha = -INF;
	int beta = INF;
	nodes_count = 0;
	moves.sort(best_move);
	for (moves.it = moves.begin(); moves.it != moves.end(); moves.it++) {
	    Move move = *moves.it;
	    make_move(move);
	    if (is_check(player)) { // Illegal move
		undo_move(*moves.it);
		continue;
	    }
	    score = -search(-beta, -alpha, ply - 1);
	    undo_move(*moves.it);
	    if (score > alpha) {
		alpha = score;
		best_move = move;
		double time = double(clock() - start) / CLOCKS_PER_SEC;
		print_search(ply, alpha, time, nodes_count);
	    } 
	    if (!best_move.is_null()) {
		tt.save(current_node().hash(), alpha, EXACT, ply, best_move);
	    }
	}
	double time = double(clock() - start) / CLOCKS_PER_SEC;
	print_search(ply, alpha, time, nodes_count);
    }
    return best_move;
}
