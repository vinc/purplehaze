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
#include "search.h"
#include "eval.h"

using namespace std;
unsigned int Game::perft(int depth) {
    if (depth == 0) return 1;
    unsigned int nodes = 0;
    Color c = current_node().get_turn_color();
    bool use_lazy_generation = false; // Useless overhead in perft()
    Moves moves(board, pieces, current_node(), use_lazy_generation);
    Move move;
    while (!(move = moves.next()).is_null()) {
    	make_move(move);
	if (!is_check(c)) nodes += perft(depth - 1);
	undo_move(move);
    }
    return nodes;
}

int Game::q_search(int alpha, int beta, int depth, int ply) {
    int score;
    if (time.poll(nodes_count)) return 0;

    int stand_pat = eval(alpha, beta);
    if (ply >= MAX_PLY) return stand_pat;
    
    if (stand_pat >= beta) return stand_pat; // Beta cut-off

    // Delta pruning
    int delta = PIECE_VALUE[QUEEN]; // TODO: Switch off in late endgame
    if (stand_pat < alpha - delta) return alpha;
    
    if (alpha < stand_pat) alpha = stand_pat; // New alpha

    Color player = current_node().get_turn_color();
    
    Moves moves(board, pieces, current_node());    
    Move move;
    while (!(move = moves.next()).is_null()) {
	if (moves.get_state() > GOOD_CAPTURES) break; // Skip bad captures	
	make_move(move);

	if (is_check(player)) { // Illegal move
	    undo_move(move);
	    continue;
	}
	
	score = -q_search(-beta, -alpha, depth - 1, ply + 1);
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
/*
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
*/

template<NodeType node_type>
int Game::pv_search(int alpha, int beta, int depth, int ply) {
    if (time.poll(nodes_count)) return 0;
    if (depth <= 0) return q_search(alpha, beta, 0, ply + 1); // Quiescence
    if (tree.has_repetition_draw()) return 0; // Repetition draw rules

    int score = -INF;
    int old_alpha = alpha;
    Node pos = current_node();
    int best_score = -INF;
    Move best_move;

#ifdef TT
    // Lookup in Transposition Table
    Transposition trans = tt.lookup(pos.hash());
    if (!trans.is_empty()) {
	if (depth <= trans.get_depth()) {
	    //int tr_score = value_from_trans(trans.get_value(), ply);
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
       
    Color player = pos.get_turn_color();
    bool is_in_check = is_check(player);
    bool is_null_move = !pos.get_null_move_right(); // No more than one
    bool is_pv = (node_type == PV);
    
    // Check Extension
    if (is_in_check) ++depth;

#ifdef NMP
    // Null Move Pruning
    bool null_move_allowed = !is_in_check && !is_null_move && !is_pv;
    
    int nb_pieces = pieces.get_nb_pieces(player);
    if (null_move_allowed && depth > NMP_DEPTH && nb_pieces < 3) {
	Move null_move;
	make_move(null_move);
	current_node().set_null_move_right(false); // Forbide more than one null move
	int r_depth = depth - R_ADAPT(depth, nb_pieces) - 1;
	score = -pv_search<node_type>(-beta, -beta + 1, r_depth, ply + 1);
	undo_move(null_move);
	if (score >= beta) {
	    //if (score >= INF - MAX_PLY) return beta;
	    return score;
	}
    }
    else if (is_null_move) {
	// Next move we will again have the right to do another null-move
	pos.set_null_move_right(true);
    }
#endif

    // Internal Iterative Deepening
    if (depth > IID_DEPTH && best_move.is_null() && !is_null_move && is_pv) {
	Moves moves(board, pieces, current_node());
	int internal_best_score = -INF;
	Move move;
	while (!(move = moves.next()).is_null()) {
	    make_move(move);
	    if (is_check(player)) {
		undo_move(move);
		continue;
	    }
	    score = -pv_search<PV>(-beta, -alpha, depth / 2, ply + 1);
	    undo_move(move);
	    if (score > internal_best_score) {
		internal_best_score = score;
		best_move = move;
	    }
	}
	// TODO Call pv_search directly and find the best move in TT?
	//score = -pv_search<PV>(alpha, beta, depth / 2, ply);
	//Move trans_move = tt.lookup(pos.hash()).get_best_move();
	//assert(trans_move == best_move); // trans_move is sometime null
    }

    bool legal_move_found = false;
    bool is_principal_variation = true;
    
    Moves moves(board, pieces, current_node());
    moves.add(best_move, BEST);
    
    // Killer moves need pseudo legality checking before we can use them,
    // but they can cause a cut-off and dispense to generate quiet moves
    // so it's worth it.
    for (int i = 0; i < MAX_KILLERS; ++i) { 
	Move killer = get_killer_move(depth, i);
	if (is_legal(killer)) moves.add(killer, KILLERS);
    }
    
    Move move;
    while (!(move = moves.next()).is_null()) {
	//assert(is_legal(move) || assert_msg(
	//    debug_move(move) << debug_killers(depth)));	

	if (move.is_capture()) {
	    if (board.get_piece(move.get_dest()).get_type() == KING) {
		return INF - ply; // Checkmate
	    }
	}
	make_move(move);
	if (is_check(player)) { // Skip illegal move
	    undo_move(move);
	    continue;
	}
	legal_move_found = true;

	// PVS code from http://www.talkchess.com/forum/viewtopic.php?t=26974
	if (is_principal_variation) {
	    best_score = -pv_search<PV>(-beta, -alpha, depth - 1, ply + 1);

	    undo_move(move);
	    if (best_score > alpha) {
		if (best_score >= beta) { // Beta cut-off
		    // Update killer moves
		    if (!move.is_capture()) set_killer_move(depth, move);
		    
		    best_move = move;
		    goto transposition;
		} 
		alpha = best_score;
	    } 
	    is_principal_variation = false;
	}
	else {

	    bool is_giving_check = is_check(Color(!player));

	    // Futility Pruning
	    if (depth <= FUTILITY_DEPTH && 
		!is_in_check && !is_giving_check &&
		!is_killer_move(depth, move) &&
		!move.is_capture() && !move.is_promotion()) {
		    // Using an array of margins is an idea from Crafty
		    score = material_eval() + FUTILITY_MARGINS[depth];
		    if (score < alpha) {
			if (score > best_score) best_score = score;
			undo_move(move);
			continue;
		    }
	    }

	    // Late Move Reduction
	    if (depth > LMR_DEPTH && // TODO find the best minimal depth
		!is_in_check && !is_giving_check &&
		!is_killer_move(depth, move) &&
		!move.is_capture() && !move.is_promotion()) {
		
		// Do the search at a reduced depth
		score = -pv_search<ALL>(-alpha - 1, -alpha, depth - 2, ply + 1);
	    }
	    else {
		score = -pv_search<ALL>(-alpha - 1, -alpha, depth - 1, ply + 1);
	    }
	    
	    // Re-search
	    if (alpha < score && score < beta) {
		score = -pv_search<ALL>(-beta, -alpha, depth - 1, ply + 1);
		if (alpha < score) {
		    alpha = score;
		}
	    }	

	    undo_move(move);
	    if (time.poll(nodes_count)) return 0;
	    if (score > best_score) { // Found a new best move
		best_score = score;
		best_move = move;
		if (score >= beta) {// Sufficient to cause a cut-off?
		    // Update killer moves
		    if (!move.is_capture()) set_killer_move(depth, move);
		    
		    goto transposition;
		} 
	    } 
	}
    }
    if (time.poll(nodes_count)) return 0;
    if (!legal_move_found) { // End of game?
	if (is_in_check) return -INF + ply; // Checkmate
	else return 0; // Stalemate
    }
    
    // Store the search to Transposition Table
    transposition:
	if (depth >= trans.get_depth()) {
	    //int value = value_to_trans(best_score, ply);
	    int value = best_score;
	    Bound bound = (best_score >= beta ? LOWER :
			  (best_score <= old_alpha ? UPPER : EXACT));
	    tt.save(pos.hash(), value, bound, depth, best_move);
	}

    return best_score;
}

Move Game::root(int max_depth) {
    time.start_thinking(current_node().get_ply());
    Color player = current_node().get_turn_color();
    print_thinking_header();
    nodes_count = 0;
    int best_score = 0;
    Move best_move;
    assert(max_depth <= MAX_PLY);
    int id; // Iteration of Depth
    int best_scores[MAX_PLY];
    for (id = 1; id < max_depth; ++id) { // Iterative Deepening
	int score;
	int alpha = -INF;
	int beta = INF;
	if (time.is_out_of_time()) break; // Do not start this ply if no time
	if (time.get_allocated_time() - time.get_elapsed_time() < 100) {
	    // Decrease polling interval if <1s left
	    time.set_polling_interval(100000);
	}
	// Mate pruning
	if (id > 6) {
	    bool is_mate = true;
	    for (int i = 1; i < 4; ++i) {
		int val = best_scores[id - i];
		if (-INF + MAX_PLY < val && val < INF - MAX_PLY) {
		    is_mate = false;
		}
	    }
	    if (is_mate) break; // The position was mate in the 3 previous ply
	}

	Moves moves(board, pieces, current_node());
	moves.add(best_move, BEST);    
	Move move;
	for (int i = 0; !(move = moves.next()).is_null(); ++i) {
	    make_move(move);
	    if (is_check(player)) { // Skip illegal move
		undo_move(move);
		continue;
	    }
	    //NodeType node_type = (i == 0 ? PV : ALL);
	    //score = -pv_search<node_type>(-beta, -alpha, id - 1, 1);
	    if (i == 0) score = -pv_search<PV>(-beta, -alpha, id - 1, 1);
	    else score = -pv_search<ALL>(-beta, -alpha, id - 1, 1);
	    undo_move(move);
	    //print_thinking(id, score, move);
	    if (time.is_out_of_time()) break; // Discard this move
	    if (score > alpha) {
		alpha = score;
		best_score = score;
		best_move = move;
		if (nodes_count > 200000) { // Save CPU time at the beginning
		    print_thinking(id, alpha, best_move);
		}
	    } 
	}
	if (time.is_out_of_time()) {
	    // TODO Restore best_move and best_score from previous ply?
	    break; // Discard this ply
	}
	if (!best_move.is_null()) {
	    tt.save(current_node().hash(), alpha, EXACT, id, best_move);
	}
	best_scores[id] = best_score;
	//print_thinking(id, best_score, best_move);
    }
    if (!best_move.is_null()) {
	print_thinking(id, best_score, best_move);
    }
    return best_move;
}
