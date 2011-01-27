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

void print_search(int ply, int score, double time, int nodes, Move m) {
    if (ply) cout << setw(4) << ply;
    else cout << setw(4) << ' ';
    cout << setw(WIDE) << score;
    cout << setw(WIDE) << setprecision(3) << time;
    if (nodes >= 1000000) {
	cout << setw(WIDE - 1) << setprecision(3) << nodes / 1000000.0 << "M";
    }
    else {
	cout << setw(WIDE) << nodes;
    }
    //if (m.is_null()) cout << setw(WIDE) << "#";
    //else cout << setw(WIDE) << m;
    cout << setw(WIDE) << m << endl;
}

int Game::perft(int depth) {
    int nodes_count = 0;
    Color c = current_node().get_turn_color();
    if (depth == 0) return 1;
    Moves moves = movegen();
    //for (moves.it = moves.begin(); moves.it != moves.end(); moves.it++) {
    //	make_move(*moves.it);
    for (int i = 0; i < moves.size(); ++i) {
    	make_move(moves.at(i));

	if (!is_check(c)) {
	    nodes_count += perft(depth - 1);
	}
	undo_move(moves.at(i));
    }
    return nodes_count;
}

int Game::quiescence_search(int alpha, int beta, int depth) {
    int score;
    int stand_pat = eval();

    if (depth < -MAX_DEPTH) return stand_pat;
    if (stand_pat >= beta) return beta; // Beta cut-off
    if (alpha < stand_pat) alpha = stand_pat; // New alpha

    Moves moves = movegen(true);
    Color player = current_node().get_turn_color();
    //for (moves.it = moves.begin(); moves.it != moves.end(); moves.it++) {
    for (int i = 0; i < moves.size(); ++i) {
	const Move& move = moves.at(i);
	make_move(move);

	if (is_check(player)) { // Illegal move
	    undo_move(move);
	    continue;
	}
	
	score = -quiescence_search(-beta, -alpha, depth - 1);
	undo_move(move);
	if (score >= beta) {
	    return beta;
	} 
	if (score > alpha) {
	    alpha = score;
	} 
    }
    return alpha;
}

int Game::alphabeta_search(int alpha, int beta, int depth) {
    if (depth == 0) return quiescence_search(alpha, beta, 0);

    int score; //= -INF;
    int old_alpha = alpha;
    //int best_score = -INF;
    Color player = current_node().get_turn_color();
    Moves moves = movegen();
    Move best_move;

    //Transposition* ptr_trans = tt.lookup(current_node().hash());
    Transposition trans = tt.lookup(current_node().hash());
    if (trans.get_bound() != UNDEF_BOUND) {
	if (trans.get_depth() >= depth) {
	    //cout << "trans" << endl;
	    int trans_score = trans.get_value();
	    switch (trans.get_bound()) {
		case EXACT: return trans_score;
		case UPPER:
		    //if (trans_score <= alpha) return trans_score;
		    if (trans_score < beta) beta = trans_score;
		    break;
		case LOWER: 
		    //if (trans_score >= beta) return trans_score;
		    if (trans_score > alpha) alpha = trans_score;
		    break;
		default: assert(false);
	    }
	    if (alpha >= beta) return trans_score;
	}
	Move bm = trans.get_best_move();
	//cout << "Try to get bm from tt... Bound: ";
	//cout << trans.get_bound() << endl;
	if (!bm.is_null()) {
	    //cout << "Got bm from tt: " << bm << endl;
	    best_move = bm;
	}
    }

    bool legal_move_found = false;
    moves.sort(best_move);
    //for (moves.it = moves.begin(); moves.it != moves.end(); moves.it++) {
    //	Move move = *moves.it;
    for (int i = 0; i < moves.size(); ++i) {
	const Move& move = moves.at(i);
	make_move(move);

	if (is_check(player)) { // Illegal move
	    undo_move(move);
	    continue;
	}

	legal_move_found = true;
	
	score = -alphabeta_search(-beta, -alpha, depth - 1);
	
	undo_move(move);
	
	if (score >= beta) {
	    tt.save(current_node().hash(), score, LOWER, depth, move);
	    //cout << "Save LOWER!" << endl;
	    return beta; // FIXME Should be score
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

    //tt.save(current_node().hash(), alpha, alpha, beta, depth, best_move);
    if (old_alpha == alpha) {
	//cout << "Save UPPER!" << endl;
	tt.save(current_node().hash(), alpha, UPPER, depth, Move());
    }
    else if (!best_move.is_null()) {
	//cout << "Save EXACT!" << endl;
	tt.save(current_node().hash(), alpha, EXACT, depth, best_move);
    }

    return alpha;
}

int Game::principal_variation_search(int alpha, int beta, int depth) {
    if (depth == 0) return quiescence_search(alpha, beta, 0);
    //int old_alpha = alpha;
    int score = -INF;
    int best_score = -INF;
    Move best_move;
    const Transposition& trans = tt.lookup(current_node().hash());
    if (trans.get_bound() != UNDEF_BOUND) {
	if (trans.get_depth() >= depth) {
	    int tr_score = trans.get_value();
	    switch (trans.get_bound()) {
		case EXACT: return tr_score;
		case UPPER: if (tr_score < beta) beta = tr_score; break;
		case LOWER: if (tr_score > alpha) alpha = tr_score; break;
		default: assert(false);
	    }
	    if (alpha >= beta) return tr_score;
	}
	const Move& bm = trans.get_best_move();
	if (!bm.is_null()) best_move = bm;
    }

    Color player = current_node().get_turn_color();
    bool legal_move_found = false;
    bool is_principal_variation = true;
    Moves moves = movegen();
    moves.sort(best_move);
    for (int i = 0; i < moves.size(); ++i) {
	const Move& move = moves.at(i);
	make_move(move);

	if (is_check(player)) { // Illegal move
	    undo_move(move);
	    continue;
	}
	legal_move_found = true;
	
	if (is_principal_variation) {
	    best_score = -principal_variation_search(-beta, -alpha, depth - 1);
	    undo_move(move);
	    if (best_score > alpha) {
		if (best_score >= beta) {
		    tt.save(current_node().hash(), best_score, LOWER, depth, 
			    move);
		    return best_score;
		} 
		alpha = score;
	    } 
	    is_principal_variation = false;
	}
	else {
	    score = -principal_variation_search(-alpha - 1, -alpha, depth - 1);
	    if (alpha < score && score < beta) {
		score = -principal_variation_search(-beta, -alpha, depth - 1);
		if (alpha < score) {
		    alpha = score;
		}
	    }		
	    undo_move(move);
	    if (score > best_score) {
		if (score >= beta) {
		    tt.save(current_node().hash(), score, LOWER, depth, move);
		    return score;
		} 
		best_score = score;
		best_move = move;
	    } 
	}
    }
    if (!legal_move_found) {
	if (is_check(player)) return -INF + 100 - depth; // Checkmate
	else return 0; // Stalemate
    }

    //if (old_alpha == alpha) {
    //	tt.save(current_node().hash(), alpha, UPPER, depth, Move());
    //}
    else if (!best_move.is_null()) { // TODO Should not be null
	tt.save(current_node().hash(), best_score, EXACT, depth, best_move);
    }

    return best_score;
}

Move Game::root(int max_depth) {
    Move best_move;
    Color player = current_node().get_turn_color();
    Moves moves = movegen();
    clock_t start = clock();

    nodes_count = 0;
    print_search_legend();
    for (int ply = 1; ply < max_depth; ++ply) {
	int score;
	int alpha = -INF;
	int beta = INF;
	moves.sort(best_move);
	for (int i = 0; i < moves.size(); ++i) {
	    const Move& move = moves.at(i);
	    make_move(move);
	    if (is_check(player)) { // Illegal move
		undo_move(move);
		continue;
	    }
	    score = -principal_variation_search(-beta, -alpha, ply - 1);
	    undo_move(move);
	    if (score > alpha) {
		alpha = score;
		best_move = move;
		double time = double(clock() - start) / CLOCKS_PER_SEC;
		print_search(0, alpha, time, nodes_count, best_move);
	    } 
	}
	if (!best_move.is_null()) {
	    tt.save(current_node().hash(), alpha, EXACT, ply, best_move);
	}
	double time = double(clock() - start) / CLOCKS_PER_SEC;
	print_search(ply, alpha, time, nodes_count, best_move);
    }
    return best_move;
}
