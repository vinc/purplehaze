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

#include <cassert>
#include <iostream>
#include <iomanip>

#include "game.h"
#include "search.h"
#include "eval.h"

inline bool Game::is_dangerous(Move m)
{
    // current_position() is used assuming
    // that the move as already been made.

    if (board[m.dest()].is(PAWN)) {
        const Color c = !current_position().side();
        if (m.dest_rank() + RANK_6 * c == RANK_7) {
            return true;
        }
    }

    /*
    if (m.is_capture()) {
        const Piece capture = current_position().capture();
        if (capture.type() != PAWN) {
            return true;
        }
    }

    return m.is_promotion();
    */

    return m.is_capture() || m.is_promotion();
}

unsigned long long int Game::perft(unsigned int depth)
{
    if (depth == 0) {
        return 1;
    }
    unsigned long long int nodes = 0;
    Color c = current_position().side();
    bool use_lazy = false; // Lazy moves generation is not usefull in perft()
    Moves moves(board, pieces, current_position(), search_moves, use_lazy);
    Move move;
    while (!(move = moves.next()).is_null()) {
#ifdef LEGAL
        if (depth == 1) {
            if (is_legal(move)) {
                ++nodes; // TODO: Finish is_legal()
            }
        } else {
#endif
            make_move(move);
            if (!is_check(c)) {
                nodes += perft(depth - 1);
            }
            undo_move(move);
#ifdef LEGAL
        }
#endif
    }
    return nodes;
}

int Game::quiescence(int alpha, int beta, int depth, const int ply)
{
    if (time.poll(nodes_count)) {
        return 0;
    }

    const int stand_pat = eval(alpha, beta);
    if (ply >= MAX_PLY) {
        return stand_pat;
    }

    if (stand_pat >= beta) {
        return stand_pat; // Beta cut-off
    }

    // Delta pruning
    const int delta = PIECE_VALUE[QUEEN]; // TODO: Switch off in late endgame
    if (stand_pat < alpha - delta) {
        return alpha;
    }

    if (alpha < stand_pat) {
        alpha = stand_pat; // New alpha
    }

    Color player = current_position().side();

    Moves moves(board, pieces, current_position(), search_moves);
    Move move;
    while (!(move = moves.next()).is_null()) {
        if (moves.state() > GOOD_CAPTURES) {
            break; // Skip bad captures
        }
        make_move(move);

        if (is_check(player)) { // Illegal move
            undo_move(move);
            continue;
        }

        const int score = -quiescence(-beta, -alpha, depth - 1, ply + 1);
        undo_move(move);
        if (time.poll(nodes_count)) {
            return 0;
        }
        if (score >= beta) {
            return score;
        }
        if (score > alpha) {
            alpha = score;
        }
    }
    if (time.poll(nodes_count)) {
        return 0;
    }
    return alpha;
}

template<NodeType node_type>
int Game::search(int alpha, int beta, int depth, const int ply)
{
    if (time.poll(nodes_count)) {
        return 0;
    }
    if (depth <= 0) {
        return quiescence(alpha, beta, 0, ply + 1); // Quiescence
    }
    if (tree.has_repetition_draw()) {
        return 0; // Repetition draw rules
    }

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
        const bool discard = pos.hash() == 0 && trans.bound() == UNDEF_BOUND;

        if (/*!is_pv &&*/ depth <= trans.depth() && !discard) {
            const int tr_score = trans.value();
            switch (trans.bound()) {
            case LOWER:
                if (tr_score > alpha) {
                    alpha = tr_score;
                }
                break;
            case UPPER:
                if (tr_score < beta) {
                    beta = tr_score;
                }
                break;
            case EXACT:
                return tr_score; // Already searched node
            default:
                assert(false);
            }
            if (alpha >= beta) {
                return tr_score; // TT cause a cut-off
            }
        }

        // If the transposition does not contain the best move,
        // best_move.is_null() will be true.
        best_move = trans.best_move();
    }

    const Color player = pos.side();
    const bool is_in_check = is_check(player);
    const bool is_null_move = !pos.can_null_move(); // No more than one

#ifndef NCE
    // Check Extension
    if (is_in_check) {
        ++depth;
    }
#endif

#ifndef NNMP
    // Null Move Pruning
    const int nb_pieces = pieces.count(player);
    const int nb_pawns = pieces.count(player, PAWN);
    const bool is_pawn_ending = nb_pieces == nb_pawns + 1; // Pawns + king

    const bool nmp_allowed =
        !is_in_check &&
        !is_null_move &&
        !is_pv &&
        !is_pawn_ending;

    if (nmp_allowed && depth > NMP_DEPTH && nb_pieces > 3) {
        Move null_move;
        make_move(null_move);
        current_position().set_null_move_right(false); // No consecutive NM
        const int r = R_ADAPT(depth, nb_pieces);
        score = -search<node_type>(-beta, -beta + 1, depth - r - 1, ply + 1);
        undo_move(null_move);
        if (score >= beta) {
            return score;
        }
    } else if (is_null_move) {
        // Next move we will again have the right to do another null-move
        pos.set_null_move_right(true);
    }
#endif

#ifndef NIID
    // Internal Iterative Deepening
    const bool iid_allowed = !is_null_move && is_pv;
    if (iid_allowed && depth > IID_DEPTH && best_move.is_null()) {
        search<PV>(alpha, beta, depth / 2, ply);
        best_move = tt.lookup(pos.hash(), &is_empty).best_move();
    }
#endif

    bool legal_move_found = false;
    bool is_principal_variation = true;

    Moves moves(board, pieces, current_position(), search_moves);
    moves.add(best_move, BEST);

    // Killer moves need pseudo legality checking before we can use them,
    // but they can cause a cut-off and dispense to generate quiet moves
    // so it's worth it.
    for (const Move &killer_move : killers(depth)) {
        if (is_legal(killer_move)) {
            moves.add(killer_move, KILLERS);
        }
    }

    Move move;
    while (!(move = moves.next()).is_null()) {
        if (move.is_capture()) {
            if (board[move.dest()].is(KING)) {
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
                    if (!move.is_capture()) {
                        set_killer(move, depth);
                    }

                    best_move = move;
                    goto transposition;
                }
                alpha = best_score;
            }
            is_principal_variation = false;
        } else {
            const bool is_giving_check = is_check(!player);

#ifndef NFP
            // Futility Pruning
            const bool fp_allowed =
                !is_in_check &&
                !is_giving_check &&
                !is_killer(move, depth) &&
                !is_dangerous(move) &&
                !move.is_castle() &&
                legal_move_found &&
                best_score < INF - MAX_PLY &&
                pieces.count(!player) > 3;

            if (fp_allowed && depth <= FUTILITY_DEPTH) {
                // Using an array of margins is an idea from Crafty
                score = material_eval() + FUTILITY_MARGINS[depth];
                if (score < alpha) {
                    undo_move(move);
                    continue;
                }
            }
#endif

            int r = 0; // Depth reduction

#ifndef NLMR
            // Late Move Reduction
            const bool lmr_allowed =
                !is_in_check &&
                !is_giving_check &&
                !is_killer(move, depth) &&
                !move.is_capture() &&
                !move.is_promotion();

            if (lmr_allowed && depth > LMR_DEPTH) {
                ++r; // Do the search at a reduced depth
            }
#endif

            // Search
            score = -search<ALL>(-alpha - 1, -alpha, depth - r - 1, ply + 1);

            // Re-search
            if (alpha < score && score < beta) {
                score = -search<ALL>(-beta, -alpha, depth - 1, ply + 1);
                if (alpha < score) {
                    alpha = score;
                }
            }

            undo_move(move);
            if (time.poll(nodes_count)) {
                return 0;
            }
            if (score > best_score) { // Found a new best move
                best_score = score;
                best_move = move;
                if (score >= beta) { // Sufficient to cause a cut-off?
                    if (!move.is_capture()) {
                        set_killer(move, depth); // Update killer moves
                    }

                    goto transposition;
                }
            }
        }
    }
    if (time.poll(nodes_count)) {
        return 0;
    }
    if (!legal_move_found) { // End of game?
        if (is_in_check) {
            return -INF + ply; // Checkmate
        } else {
            return 0; // Stalemate
        }
    }

transposition:
    // Store the search to Transposition Table
    //assert(!best_move.is_null());
    if (depth >= trans.depth() /*&& !is_null_move*/) {
        const int value = best_score;
        const Bound bound = (best_score >= beta ? LOWER :
                                (best_score <= old_alpha ? UPPER : EXACT));
        if (bound == UPPER) {
            best_move = Move(); // Don't store best move
        }
        tt.save(pos.hash(), value, bound, depth, best_move);
    }
    return best_score;
}

Move Game::root(int max_depth)
{
    assert(max_depth <= MAX_PLY);

    nodes_count = 0;
    search_moves.clear();
    time.start_thinking(tree.ply());
    print_thinking_header();

    int best_score = 0;
    Move best_move;
    int best_scores[MAX_PLY];
    int depth;
    for (depth = 1; depth < max_depth; ++depth) { // Iterative Deepening
        int score;
        int alpha = -INF;
        int beta = INF;
        if (time.poll(nodes_count)) {
            break; // Do not start a new ply
        }

        // Increase poll frequency as time is running out
        const int time_remaining = time.allocated() - time.elapsed();
        if (time_remaining < 1000) {
            time.set_polling_interval(time_remaining * 512);
        }

        // Mate pruning
        if (depth > 6) {
            bool is_mate = true;
            for (int i = 1; i < 4; ++i) {
                int val = best_scores[depth - i];
                if (-INF + MAX_PLY < val && val < INF - MAX_PLY) {
                    is_mate = false;
                }
            }
            if (is_mate) {
                break; // The position was mate in the 3 previous plies
            }
        }

        Moves moves(board, pieces, current_position(), search_moves);
        moves.add(best_move, BEST);
        Move move;
        int nb_moves;
        for (nb_moves = 1; !(move = moves.next()).is_null(); ++nb_moves) {
            make_move(move);
            if (is_check(!current_position().side())) { // Skip illegal move
                undo_move(move);
                --nb_moves;
                continue;
            }
            if (nb_moves == 1) {
                score = -search<PV>(-beta, -alpha, depth - 1, 1);
            } else {
                score = -search<ALL>(-beta, -alpha, depth - 1, 1);
            }
            undo_move(move);
            if (time.poll(nodes_count)) {
                break; // Discard the move
            }
            if (score > alpha) {
                alpha = score;
                best_score = score;
                best_move = move;
                if (nodes_count > 200000) { // Save CPU time at the beginning
                    print_thinking(depth, alpha, best_move);
                }
            }
        }
        if (time.poll(nodes_count)) {
            // TODO: restore best_move and best_score from previous ply?
            break; // Discard this ply
        }
        if (!best_move.is_null()) {
            tt.save(current_position().hash(), alpha, EXACT, depth, best_move);
        }
        best_scores[depth] = best_score;

        if (nb_moves == 1) { // If only one move allowed,
            break;           // no need to do iterative deepening
        }
    }
    if (!best_move.is_null()) {
        print_thinking(depth, best_score, best_move);
    }
    return best_move;
}
