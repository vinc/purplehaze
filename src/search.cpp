/* Copyright (C) 2007-2011 Vincent Ollivier
 *
 * Purple Haze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Purple Haze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <iomanip>

#include "game.h"
#include "search.h"
#include "eval.h"

unsigned long long int Game::perft(unsigned int depth)
{
    if (depth == 0) return 1;
    unsigned int nodes = 0;
    Color c = current_position().get_turn_color();
    bool use_lazy = false; // Lazy moves generation is not usefull in perft()
    Moves moves(board, pieces, current_position(), search_moves, use_lazy);
    Move move;
    while (!(move = moves.next()).is_null()) {
#ifdef LEGAL
        if (depth == 1) {
            if (is_legal(move)) ++nodes; // TODO: Finish is_legal()
        } else {
#endif
            make_move(move);
            if (!is_check(c)) nodes += perft(depth - 1);
            undo_move(move);
#ifdef LEGAL
        }
#endif
    }
    return nodes;
}

int Game::quiescence(int alpha, int beta, int depth, const int ply)
{
    if (time.poll(nodes_count)) return 0;

    const int stand_pat = eval(alpha, beta);
    if (ply >= MAX_PLY) return stand_pat;

    if (stand_pat >= beta) return stand_pat; // Beta cut-off

    // Delta pruning
    const int delta = PIECE_VALUE[QUEEN]; // TODO: Switch off in late endgame
    if (stand_pat < alpha - delta) return alpha;

    if (alpha < stand_pat) alpha = stand_pat; // New alpha

    Color player = current_position().get_turn_color();

    Moves moves(board, pieces, current_position(), search_moves);
    Move move;
    while (!(move = moves.next()).is_null()) {
        if (moves.get_state() > GOOD_CAPTURES) break; // Skip bad captures
        make_move(move);

        if (is_check(player)) { // Illegal move
            undo_move(move);
            continue;
        }

        const int score = -quiescence(-beta, -alpha, depth - 1, ply + 1);
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

template<NodeType node_type>
int Game::search(int alpha, int beta, int depth, const int ply)
{
    if (time.poll(nodes_count)) return 0;
    if (depth <= 0) return quiescence(alpha, beta, 0, ply + 1); // Quiescence
    if (tree.has_repetition_draw()) return 0; // Repetition draw rules

    int score = -INF;
    const int old_alpha = alpha;
    Position pos = current_position();
    int best_score = -INF;
    Move best_move;
    const bool is_pv = (node_type == PV);

    // Lookup in Transposition Table
    bool is_empty;
    Transposition trans = tt.lookup(pos.hash(), &is_empty);
    if (!is_empty) {
        // FIXME Avoid a potential bug with tt.lookup()
        const bool discard = pos.hash() == 0 &&
                             trans.get_bound() == UNDEF_BOUND;

        if (/*!is_pv &&*/ depth <= trans.get_depth() && !discard) {
            const int tr_score = trans.get_value();
            switch (trans.get_bound()) {
                case EXACT: return tr_score; // Already searched node
                case UPPER: if (tr_score < beta) beta = tr_score; break;
                case LOWER: if (tr_score > alpha) alpha = tr_score; break;
                default: assert(false);
            }
            if (alpha >= beta) return tr_score; // TT cause a cut-off
        }
        Move bm = trans.get_best_move();
        if (!bm.is_null()) best_move = bm; // Save the best move
    }

    const Color player = pos.get_turn_color();
    const bool is_in_check = is_check(player);
    const bool is_null_move = !pos.get_null_move_right(); // No more than one

    // Check Extension
    if (is_in_check) ++depth;

#ifdef NMP
    // Null Move Pruning
    const bool null_move_allowed = !is_in_check && !is_null_move && !is_pv;

    const int nb_pieces = pieces.count(player);
    if (null_move_allowed && depth > NMP_DEPTH && nb_pieces > 3) {
        Move null_move;
        make_move(null_move);
        current_position().set_null_move_right(false); // No consecutive NM
        const int r_depth = depth - R_ADAPT(depth, nb_pieces) - 1;
        score = -search<node_type>(-beta, -beta + 1, r_depth, ply + 1);
        undo_move(null_move);
        if (score >= beta) {
            return score;
        }
    } else if (is_null_move) {
        // Next move we will again have the right to do another null-move
        pos.set_null_move_right(true);
    }
#endif

    // Internal Iterative Deepening
    if (depth > IID_DEPTH && best_move.is_null() && !is_null_move && is_pv) {
        Moves moves(board, pieces, current_position(), search_moves);
        int internal_best_score = -INF;
        Move move;
        while (!(move = moves.next()).is_null()) {
            make_move(move);
            if (is_check(player)) {
                undo_move(move);
                continue;
            }
            score = -search<PV>(-beta, -alpha, depth / 2, ply + 1);
            undo_move(move);
            if (score > internal_best_score) {
                internal_best_score = score;
                best_move = move;
            }
        }
        // TODO Call search directly and find the best move in TT?
        /*
        tt.save(pos.hash(), 0, UNDEF_BOUND, 0, Move());
        Transposition trans_test = tt.lookup(pos.hash());
        pos.set_null_move_right(false);
        score = search<PV>(alpha, beta, depth / 2, ply);
        if (score <= alpha) {
            // Avoid storing lower bound in TT
            score = search<PV>(-INF, beta, depth / 2, ply);
        }
        pos.set_null_move_right(true);
        Move trans_move = tt.lookup(pos.hash()).get_best_move();
        if (trans_move != best_move) {
            cout << "score=" << score << " alpha=" << alpha << " ";
            cout << trans_test.to_string() << " ";
            cout << internal_best_score << " " << best_move << " ";
            cout << tt.lookup(pos.hash()).to_string() << endl;
        }
        //assert(trans_move == best_move); // trans_move is sometime null
        */
    }

    bool legal_move_found = false;
    bool is_principal_variation = true;

    Moves moves(board, pieces, current_position(), search_moves);
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
            best_score = -search<PV>(-beta, -alpha, depth - 1, ply + 1);

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
        } else {
            const bool is_giving_check = is_check(!player);

            // Futility Pruning
            if (depth <= FUTILITY_DEPTH &&
                //!best_move.is_null() &&
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
                //!best_move.is_null() &&
                !is_in_check && !is_giving_check &&
                !is_killer_move(depth, move) &&
                !move.is_capture() && !move.is_promotion()) {

                // Do the search at a reduced depth
                score = -search<ALL>(-alpha - 1, -alpha, depth - 2, ply + 1);
            } else {
                score = -search<ALL>(-alpha - 1, -alpha, depth - 1, ply + 1);
            }

            // Re-search
            if (alpha < score && score < beta) {
                score = -search<ALL>(-beta, -alpha, depth - 1, ply + 1);
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
        return 0; // Stalemate
    }

transposition:
    // Store the search to Transposition Table
    //assert(!best_move.is_null());
    if (depth >= trans.get_depth() /*&& !is_null_move*/) {
        const int value = best_score;
        const Bound bound = (best_score >= beta ? LOWER :
                                (best_score <= old_alpha ? UPPER : EXACT));
        if (bound == UPPER) best_move = Move(); // Don't store best move
        tt.save(pos.hash(), value, bound, depth, best_move);
    }

    return best_score;
}

Move Game::root(int max_depth)
{
    time.start_thinking(current_position().get_ply());
    Color player = current_position().get_turn_color();
    print_thinking_header();
    nodes_count = 0;
    int best_score = 0;
    Move best_move;
    assert(max_depth <= MAX_PLY);
    int it; // Iteration of Depth
    int best_scores[MAX_PLY];
    for (it = 1; it < max_depth; ++it) { // Iterative Deepening
        int score;
        int alpha = -INF;
        int beta = INF;
        if (time.is_out_of_time()) break; // Do not start this ply if no time
        if (time.get_allocated_time() - time.get_elapsed_time() < 100) {
            // Decrease polling interval if <1s left
            time.set_polling_interval(100000);
        }
        // Mate pruning
        if (it > 6) {
            bool is_mate = true;
            for (int i = 1; i < 4; ++i) {
                int val = best_scores[it - i];
                if (-INF + MAX_PLY < val && val < INF - MAX_PLY) {
                    is_mate = false;
                }
            }
            if (is_mate) break; // The position was mate in the 3 previous ply
        }

        Moves moves(board, pieces, current_position(), search_moves);
        moves.add(best_move, BEST);
        Move move;
        int nb_moves;
        for (nb_moves = 1; !(move = moves.next()).is_null(); ++nb_moves) {
            make_move(move);
            if (is_check(player)) { // Skip illegal move
                undo_move(move);
                --nb_moves;
                continue;
            }
            // FIXME
            //NodeType node_type = (nb_moves ? PV : ALL);
            //score = -search<node_type>(-beta, -alpha, it - 1, 1);
            if (nb_moves == 1) {
                score = -search<PV>(-beta, -alpha, it - 1, 1);
            } else {
                score = -search<ALL>(-beta, -alpha, it - 1, 1);
            }
            undo_move(move);
            if (time.is_out_of_time()) break; // Discard this move
            if (score > alpha) {
                alpha = score;
                best_score = score;
                best_move = move;
                if (nodes_count > 200000) { // Save CPU time at the beginning
                    print_thinking(it, alpha, best_move);
                }
            }
        }
        if (time.is_out_of_time()) {
            // TODO: restore best_move and best_score from previous ply?
            break; // Discard this ply
        }
        if (!best_move.is_null()) {
            tt.save(current_position().hash(), alpha, EXACT, it, best_move);
        }
        best_scores[it] = best_score;

        // If there is only one legal move, no iterative deepening needed
        if (nb_moves == 1) break;
    }
    if (!best_move.is_null()) {
        print_thinking(it, best_score, best_move);
    }
    return best_move;
}
