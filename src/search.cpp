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
#include "eval.h"

using namespace std;

#define	R 2

// Adaptive Null-Move Pruning (Heinz 1999)
#define R_ADAPT(c, d) ( \
    2 + ((d) > (6 + ((pieces.get_nb_pieces(c) < 3) ? 2 : 0))))

int Game::perft(int depth) {
    if (depth == 0) return 1;
    int nodes = 0;
    Color c = current_node().get_turn_color();
    Moves moves = movegen();
    for (int i = 0; i < moves.size(); ++i) {
    	make_move(moves.at(i));
	if (!is_check(c)) nodes += perft(depth - 1);
	undo_move(moves.at(i));
    }
    return nodes;
}

int Game::quiescence_search(int alpha, int beta, int depth) {
    int score;
    if (time.poll(nodes_count)) return 0;
    int stand_pat = eval();
    if (depth < -MAX_DEPTH) return stand_pat;
    
    if (stand_pat >= beta) return stand_pat; // Beta cut-off

    // Delta pruning
    int delta = PIECE_VALUE[QUEEN]; // TODO: Switch of in late endgame
    if (stand_pat < alpha - delta) return alpha;
    
    if (alpha < stand_pat) alpha = stand_pat; // New alpha

    Moves moves = movegen(true); // Capture only
    moves.sort(board);
    Color player = current_node().get_turn_color();
    for (int i = 0; i < moves.size(); ++i) {
	if (moves.get_score(i) < 0) break; // Skip bad captures	
	Move move = moves.at(i);
	make_move(move);

	if (is_check(player)) { // Illegal move
	    undo_move(move);
	    continue;
	}
	
	score = -quiescence_search(-beta, -alpha, depth - 1);
	undo_move(move);
	if (time.poll(nodes_count)) return 0;
	if (score >= beta) {
	    return score;
	} 
	if (score > alpha) {
	    alpha = score;
	} 
    }
    if (time.poll(nodes_count)) return 0;
    return alpha;
}

/* 
 * Replaced by Principal Variation Search
 */
int Game::alphabeta_search(int alpha, int beta, int depth) {
    int score;
    int old_alpha = alpha;
    Move best_move;
    Transposition trans = tt.lookup(current_node().hash());
    if (trans.get_bound() != UNDEF_BOUND) {
	if (trans.get_depth() >= depth) {
	    int trans_score = trans.get_value();
	    switch (trans.get_bound()) {
		case EXACT: return trans_score;
		case UPPER: if (trans_score < beta) beta = trans_score; break;
		case LOWER: if (trans_score > alpha) alpha = trans_score; break;
		default: assert(false);
	    }
	    if (alpha >= beta) return trans_score;
	}
	Move bm = trans.get_best_move();
	if (!bm.is_null()) best_move = bm;
    }
    if (depth <= 0) return quiescence_search(alpha, beta, 0); // Quiescence
    if (tree.has_repetition_draw()) return 0; // Repetition draw rules
    Color player = current_node().get_turn_color();
    bool legal_move_found = false;
    Moves moves = movegen();
    moves.sort(board, best_move);
    for (int i = 0; i < moves.size(); ++i) {
	Move move = moves.at(i);
	make_move(move);
	if (is_check(player)) { // Skip illegal move
	    undo_move(move);
	    continue;
	}
	legal_move_found = true;
	score = -alphabeta_search(-beta, -alpha, depth - 1);
	undo_move(move);
	if (score >= beta) {
	    tt.save(current_node().hash(), score, LOWER, depth, move);
	    return beta; // FIXME Should it be score?
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
    Bound bound = (old_alpha <= alpha ? UPPER : EXACT); // FIXME probably wrong
    tt.save(current_node().hash(), alpha, bound, depth, best_move);
    return alpha;
}

int Game::pv_search(int alpha, int beta, int depth, NodeType node_type) {
    int score = -INF;
    int old_alpha = alpha;
    Node pos = current_node();
    int best_score = -INF;
    Move best_move;

    if (time.poll(nodes_count)) return 0;

#ifdef TT
    // Lookup in Transposition Table
    Transposition trans = tt.lookup(pos.hash());
    //Transposition trans = tt.lookup(current_node().hash());
    if (!trans.is_empty()) {
	if (trans.get_depth() >= depth) {
	    int tr_score = trans.get_value();
	    switch (trans.get_bound()) {
		case EXACT: return tr_score; // Already searched node
		case UPPER: if (tr_score < beta) beta = tr_score; break;
		case LOWER: if (tr_score > alpha) alpha = tr_score; break;
		default: assert(!"Corrupted Transposition Table");
	    }
	    if (alpha >= beta) return tr_score; // TT cause a cut-off
	}
	Move bm = trans.get_best_move();
	if (!bm.is_null()) best_move = bm; // Save the best move
    }
#endif
    
    // End of regular search?
    if (depth <= 0) return quiescence_search(alpha, beta, 0); // Quiescence
    if (tree.has_repetition_draw()) return 0; // Repetition draw rules

    Color player = pos.get_turn_color();
    //Color player = current_node().get_turn_color();
    bool is_in_check = is_check(player);

#ifdef NMP
    // Null Move Pruning
    bool is_null = pos.get_last_move().is_null();
    //bool is_null = current_node().get_last_move().is_null();
    bool is_pv = (node_type == PV_NODE);
    bool null_move_allowed = !is_in_check && !is_null && !is_pv;
    
    if (null_move_allowed && depth >= 2) {
	Move null_move;
	make_move(null_move);
	int reduced_depth = depth - R_ADAPT(player, depth) - 1;
	score = -pv_search(-beta, -beta + 1, reduced_depth, node_type);
	undo_move(null_move);
	if (score >= beta) return score;
    }
    
#endif

    bool legal_move_found = false;
    bool is_principal_variation = true;
    Moves moves = movegen();
    Move killer1 = get_killer_move(depth, 0);
    Move killer2 = get_killer_move(depth, 1);
    moves.sort(board, best_move, killer1, killer2);
    for (int i = 0; i < moves.size(); ++i) {
	Move move = moves.at(i);
	make_move(move);
	if (is_check(player)) { // Skip illegal move
	    undo_move(move);
	    continue;
	}
	legal_move_found = true;
	
	// PVS code from http://www.talkchess.com/forum/viewtopic.php?t=26974
	if (is_principal_variation) {
	    best_score = -pv_search(-beta, -alpha, depth - 1, PV_NODE);

	    undo_move(move);
	    if (best_score > alpha) {
		if (best_score >= beta) {
		    // Store the search to Transposition Table
		    tt.save(pos.hash(), best_score, LOWER, depth, move);
		    //tt.save(current_node().hash(), best_score, LOWER, depth, move);
		    
		    // Update killer moves
		    set_killer_move(depth, move);
		    
		    // Beta cut-off
		    return best_score;
		} 
		alpha = best_score;
	    } 
	    is_principal_variation = false;
	}
	else {

	    // Late Move Reduction
	    if (i > 3 && // Not for best move or killer moves
		depth > 2 && // Not near leaf
		!move.is_capture() && !move.is_promotion() &&
		!is_in_check && !is_check(Color(!player))) {
		
		// Do the search at a reduced depth
		score = -pv_search(-alpha - 1, -alpha, depth - 2, ALL_NODE);
	    }
	    else {
		score = -pv_search(-alpha - 1, -alpha, depth - 1, ALL_NODE);
	    }

	    if (alpha < score && score < beta) {
		score = -pv_search(-beta, -alpha, depth - 1, ALL_NODE);
		if (alpha < score) {
		    alpha = score;
		}
	    }		
	    undo_move(move);
	    if (time.poll(nodes_count)) return 0;
	    if (score > best_score) { // Found a new best move
		if (score >= beta) {// Sufficient to cause a cut-off?
		    // Store the search to Transposition Table
		    tt.save(pos.hash(), score, LOWER, depth, move);
		    //tt.save(current_node().hash(), score, LOWER, depth, move);
		    
		    // Update killer moves
		    set_killer_move(depth, move); // TODO update killers in PV?
		    
		    // Beta cut-off
		    return score;
		} 
		best_score = score;
		best_move = move;
	    } 
	}
    }
    if (time.poll(nodes_count)) return 0;
    if (!legal_move_found) { // End of game?
	if (is_check(player)) return -INF + 100 - depth; // Checkmate
	else return 0; // Stalemate
    }

    // Store the search to Transposition Table
    Bound bound = (best_score <= old_alpha ? UPPER : EXACT);
    tt.save(pos.hash(), best_score, bound, depth, best_move);
    //tt.save(current_node().hash(), best_score, bound, depth, best_move);

    return best_score;
}

Move Game::root(int max_depth) {
    time.start_thinking(current_node().get_ply());
    Color player = current_node().get_turn_color();
    print_thinking_header();
    nodes_count = 0;
    Move best_move;
    Moves moves = movegen();
    for (int ply = 1; ply < max_depth; ++ply) { // Iterative Deepening
	int score;
	int alpha = -INF;
	int beta = INF;
	if (time.is_out_of_time()) break; // Do not start this ply if no time
	moves.sort(board, best_move);
	for (int i = 0; i < moves.size(); ++i) {
	    Move move = moves.at(i);
	    make_move(move);
	    if (is_check(player)) { // Skip illegal move
		undo_move(move);
		continue;
	    }
	    NodeType node_type = (i == 0 ? PV_NODE : ALL_NODE);
	    score = -pv_search(-beta, -alpha, ply - 1, node_type);
	    undo_move(move);
	    if (time.is_out_of_time()) break; // Discard this move
	    if (score > alpha) {
		alpha = score;
		best_move = move;
		if (nodes_count > 500000) { // Save CPU time at the beginning
		    print_thinking(ply, alpha, best_move);
		}
	    } 
	}
	if (time.is_out_of_time()) break; // Discard this ply
	if (!best_move.is_null()) {
	    tt.save(current_node().hash(), alpha, EXACT, ply, best_move);
	}
	//print_thinking(ply, alpha, best_move);
    }
    return best_move;
}
