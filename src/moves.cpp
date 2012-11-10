/* Copyright (C) 2007-2012 Vincent Ollivier
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

#include "moves.h"
#include "board.h"
#include "position.h"

/*
 * Use lazy move generation to get the next move.
 *
 * Best and killers moves are added first to the moves array and we start
 * with the best move. We then generate the captures but only if there have
 * been no cut-off with the best move.
 *
 * MVV/LVA is used to pickup the best captures first by selection sort and
 * next we try the killer moves (stored near the beginning of the moves array)
 * before the bad captures. Which are none by the way with MVV/LVA but in
 * the future SEE will be used for that.
 *
 * Finally when there is no captures left and no cut-off occurred we generate
 * the remaining quiets moves and try them with no particular order.
 */
ExtendedMove Moves::next()
{
    if (!use_lazy_generation) {
        if (cur == 0) {
            generate(); // generate() will change the value of 'n'
        }
        if (cur == end) {
            return ExtendedMove();
        }
        return moves[cur++];
    }

    switch (generation_state) {
    case BEST:
        if (cur < size[BEST]) {
            return moves[cur++];
        }
        generation_state = GOOD_CAPTURES;
        generate(CAPTURE);
        cur = size[BEST] + size[KILLERS]; // Jump to first good capture
    case GOOD_CAPTURES:
        if (cur < (size[BEST] + size[KILLERS] + size[GOOD_CAPTURES])) {
            break;
        }
        generation_state = KILLERS;
        cur = size[BEST]; // Jump to the first killer
    case KILLERS:
        if (cur < (size[BEST] + size[KILLERS])) {
            return moves[cur++];
        }
        generation_state = BAD_CAPTURES;
        cur = size[BEST] + size[KILLERS] + size[GOOD_CAPTURES];
    case BAD_CAPTURES:
        if (cur < end) {
            break;
        }
        generation_state = QUIET_MOVES;
        generate(QUIET_MOVE);
    case QUIET_MOVES:
        if (cur < end) {
            return moves[cur++];
        }
    default:
        return ExtendedMove();
    }

    // If we are here, next() should return a capture
    assert(state() == GOOD_CAPTURES || state() == BAD_CAPTURES);

    // Find the best remaining capture by selection sort
    int max = cur;
    for (int i = cur + 1; i < end; ++i) {
        if (moves[i].value() > moves[max].value()) {
            max = i;
        }
    }

    // Swap it with the current one
    if (max != cur) {
        ExtendedMove tmp = std::move(moves[cur]);
        moves[cur] = std::move(moves[max]);
        moves[max] = std::move(tmp);
    }

    // Return it
    return moves[cur++];
}

void Moves::add(Move move, MovesState mt)
{
    if (!use_lazy_generation) {
        moves[end++] = ExtendedMove(move, 0);
        return;
    }

    if (move.is_null()) {
        return;
    }

    // Don't add again best and killer moves
    const int n = size[BEST] + size[KILLERS];
    for (int i = 0; i < n; ++i) {
        if (moves[i] == move) {
            return;
        }
    }

    // Calculate the move's score
    Score score = 0;
    switch (mt) {
    case BEST:
        score = BEST_SCORE;
        size[mt]++;
        break;
    case KILLERS:
        score = KILLERS_SCORE - size[KILLERS];
        size[mt]++;
        break;
    default:
        //assert(generation_state > KILLERS);
        //size[generation_state]++; // If move is a capture or a quiet move
        break;
    }
    switch (generation_state) {
    case GOOD_CAPTURES:
    case BAD_CAPTURES:
        score = mvv_lva_score(move);
        size[generation_state]++;
        break;
    case QUIET_MOVES:
        score = -BEST_SCORE;
        size[generation_state]++;
        break;
    default:
        //assert(mt < GOOD_CAPTURES);
        //size[mt]++; // If move is a best or killer move
        break;
    }

    // Add the move and its score to moves list
    moves[end++] = ExtendedMove(move, score);
}

Score Moves::mvv_lva_scores[][NB_PIECE_TYPES] = { { 0 } };

/**
 * MVV/LVA scores:
 *
 *     PxP =  7, PxN = 15, PxB = 23, PxR = 31, PxQ = 39, PxK = 47
 *     NxP =  6, NxN = 14, NxB = 22, NxR = 30, NxQ = 38, NxK = 46
 *     BxP =  5, BxN = 13, BxB = 21, BxR = 29, BxQ = 37, BxK = 45
 *     RxP =  4, RxN = 12, RxB = 20, RxR = 28, RxQ = 36, RxK = 44
 *     QxP =  3, QxN = 11, QxB = 19, QxR = 27, QxQ = 35, QxK = 43
 *     KxP =  2, KxN = 10, KxB = 18, KxR = 26, KxQ = 34, KxK = 42
 */
void Moves::init_mvv_lva_scores()
{
    for (const PieceType& a : PIECE_TYPES) {
        for (const PieceType& v : PIECE_TYPES) {
            mvv_lva_scores[a][v] = (8 * v) - a;
        }
    }
}

Score Moves::mvv_lva_score(Move move)
{
    assert(move.is_capture());
    PieceType a = board[move.orig()].type();
    PieceType v = move.is_en_passant() ? PAWN : board[move.dest()].type();
    return mvv_lva_scores[a][v];
}
