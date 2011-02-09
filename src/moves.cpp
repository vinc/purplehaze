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

#include "moves.h"

/*
 * Use lazy move generation to get the next move.
 *
 * Best and killers moves are added first to the moves array and we start 
 * with the best move. We then generate the captures but only if there have
 * been no cut-off with the best move.
 *
 * MVV/LVA is used to pickup the best captures first by selection sort and 
 * next we try the killer moves (stored near the begining of the moves array) 
 * before the bad captures. Which are none by the way with MVV/LVA but in 
 * the future SEE will be used for that.
 * 
 * Finaly when there is no captures left and no cut-off occured we generate
 * the remaining quiets moves and try them with no particular order.
 */
ExtendedMove Moves::next() {
    if (!use_lazy_generation) {
	if (i == 0) {
	    generate();
	}
	if (i == n) return ExtendedMove();
	return moves[i++];
    }

    switch (state) {
	case BEST:
	    if (i < size[BEST]) return moves[i++];
	    state = GOOD_CAPTURES;
	    generate(CAPTURE);
	    i = size[BEST] + size[KILLERS]; // Jump to the first good capture
	case GOOD_CAPTURES:
	    if (i < (size[BEST] + size[KILLERS] + size[GOOD_CAPTURES])) break;
	    state = KILLERS;
	    i = size[BEST]; // Jump to the first killer
	case KILLERS:
	    if (i < (size[BEST] + size[KILLERS])) return moves[i++];
	    state = BAD_CAPTURES;
	    i = size[BEST] + size[KILLERS] + size[GOOD_CAPTURES];
	case BAD_CAPTURES:
	    if (i < n) break;
	    state = QUIET_MOVES;
	    generate(QUIET_MOVE);
	case QUIET_MOVES:
	    if (i < n) return moves[i++];
	default:
	    return ExtendedMove();
    }

    // If we are here, next() should return a capture
    assert(state == GOOD_CAPTURES || state == GOOD_CAPTURES);
    
    // Find the best remaining capture by selection sort
    int max = i;
    for (int j = i + 1; j < n; ++j) {
	if (moves[j].get_score() > moves[max].get_score()) {
	    max = j;
	}
    }
    
    // Swap it with the current one
    if (max != i) {
	ExtendedMove tmp = moves[i];
	moves[i] = moves[max];
	moves[max] = tmp;
    }
    
    // Return it
    return moves[i++];
}

void Moves::add(Move move, MovesState mt) {
    if (!use_lazy_generation) {
	moves[n++] = ExtendedMove(move, 0);
	return;
    }
    
    if (move.is_null()) return;

    // Partial protection against duplicates
    for (int j = 0; j < (size[BEST] + size[KILLERS]); ++j) {
	if (moves[j] == move) return; // move has been already added
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
	    //assert(state > KILLERS);
	    //size[state]++; // If move is a capture or a quiet move
	    break;
    }
    switch (state) {
	case GOOD_CAPTURES:
	case BAD_CAPTURES:
	    score = get_mvv_lva_score(move);
	    size[state]++;
	    break;
	case QUIET_MOVES:
	    score = -BEST_SCORE;
	    size[state]++;
	    break;
	default:
	    //assert(mt < GOOD_CAPTURES);
	    //size[mt]++; // If move is a best or killer move
	    break;
    }
    
    // Add the move and its score to moves list
    moves[n++] = ExtendedMove(move, score);
}

Score Moves::mvv_lva_scores[][KING + 1] = { { 0 } };

/*
 * PxK = 94,  NxK = 92,  BxK = 90,  RxK = 88,  QxK = 86,  KxK = 84,  PxQ = 78,
 * NxQ = 76,  BxQ = 74,  RxQ = 72,  QxQ = 70,  KxQ = 68,  PxR = 62,  NxR = 60,
 * BxR = 58,  RxR = 56,  QxR = 54,  KxR = 52,  PxB = 46,  NxB = 44,  BxB = 42,
 * RxB = 40,  QxB = 38,  KxB = 36,  PxN = 30,  NxN = 28,  BxN = 26,  RxN = 24,
 * QxN = 22,  KxN = 20,  PxP = 14,  NxP = 12,  BxP = 10,  RxP =  8,  QxP =  6,
 * KxP =  4
 */
void Moves::init_mvv_lva_scores() {
    for (PieceType v = PAWN; v <= KING; v = PieceType(v + 1)) {
	for (PieceType a = PAWN; a <= KING; a = PieceType(a + 1)) {
	    mvv_lva_scores[v][a] = (16 * v) - (2 * a);
	}
    }
}

Score Moves::get_mvv_lva_score(Move move) {
    assert(move.is_capture());
    PieceType a = board.get_piece(move.get_orig()).get_type();
    PieceType v = board.get_piece(move.get_dest()).get_type();
    if (move.is_en_passant()) return mvv_lva_scores[PAWN][a];
    return mvv_lva_scores[v][a];
}

