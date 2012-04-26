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
        if (cur == 0) generate(); // generate() will change the value of 'n'

        if (cur == end) return ExtendedMove();
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

    if (move.is_null()) return;

    // Don't add again best and killer moves
    const int n = size[BEST] + size[KILLERS];
    for (int i = 0; i < n; ++i) if (moves[i] == move) return;

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

/*
 * PxK = 94,  NxK = 92,  BxK = 90,  RxK = 88,  QxK = 86,  KxK = 84,  PxQ = 78,
 * NxQ = 76,  BxQ = 74,  RxQ = 72,  QxQ = 70,  KxQ = 68,  PxR = 62,  NxR = 60,
 * BxR = 58,  RxR = 56,  QxR = 54,  KxR = 52,  PxB = 46,  NxB = 44,  BxB = 42,
 * RxB = 40,  QxB = 38,  KxB = 36,  PxN = 30,  NxN = 28,  BxN = 26,  RxN = 24,
 * QxN = 22,  KxN = 20,  PxP = 14,  NxP = 12,  BxP = 10,  RxP =  8,  QxP =  6,
 * KxP =  4
 */
void Moves::init_mvv_lva_scores()
{
    for (const PieceType& v : PIECE_TYPES) {
        for (const PieceType& a : PIECE_TYPES) {
            mvv_lva_scores[v][a] = (16 * v) - (2 * a);
        }
    }
}

Score Moves::mvv_lva_score(Move move)
{
    assert(move.is_capture());
    PieceType a = board[move.orig()].type();
    PieceType v = board[move.dest()].type();
    if (move.is_en_passant()) return mvv_lva_scores[PAWN][a];
    return mvv_lva_scores[v][a];
}
