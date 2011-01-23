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

#include "game.h"
#include "eval.h"

using namespace std;

int Game::piece_eval(Color c, PieceType t, int i) {
    Square s = pieces.get_position(c, t, i);
    const int* pcsq_table;
    switch (t) {
	case PAWN:   pcsq_table = PAWN_PCSQ;   break;
	case KNIGHT: pcsq_table = KNIGHT_PCSQ; break;
	case BISHOP: pcsq_table = BISHOP_PCSQ; break;
	case ROOK:   pcsq_table = ROOK_PCSQ;   break;
	case QUEEN:  pcsq_table = QUEEN_PCSQ;  break;
	case KING:   pcsq_table = KING_PCSQ;   break;
	default: assert(false);
    }
    switch (c) {
	case WHITE: return pcsq_table[s];
	case BLACK: return pcsq_table[FLIP[s]];
    }	
}

int Game::eval() {
    int score = 0;
    Color player = current_node().get_turn_color();
    Color opponent = Color(!player);
    
    for (PieceType t = PAWN; t <= KING; t = PieceType(t + 1)) {
	int n_player = pieces.get_nb_pieces(player, t);
	int n_opponent = pieces.get_nb_pieces(opponent, t);
	
	// Material values
	score += PIECE_VALUE[t] * n_player;
	score -= PIECE_VALUE[t] * n_opponent;

	// PCSQ Tables
	for (int i = 0; i < n_player; ++i) {
	    score += piece_eval(player, t, i);
	}
	for (int i = 0; i < n_opponent; ++i) {
	    score -= piece_eval(opponent, t, i);
	}
    }

    return score;
}
