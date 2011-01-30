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

int pawn_structure_eval(Pieces& pieces, Color player) {
    int score = 0;
    int pawns_files[2][8] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 }
    };
    for (int c = 0; c < 2; ++c) { 
	int pawns_score = 0;
	// Building the (color x file) table
	for (int i = 0; i < pieces.get_nb_pieces(Color(c), PAWN); ++i) {
	    Square s = pieces.get_position(Color(c), PAWN, i);
	    pawns_files[c][s & 7]++;
	}
	// Malus for doubled (or more) pawns
	for (int i = 0; i < 8; ++i) {
	    pawns_score += MALUS_MULTI_PAWN[pawns_files[c][i]];
	}
	if (Color(c) != player) pawns_score *= -1;
	score += pawns_score;
    }
	
    // Rook position bonus relative to pawn structure
    for (int c = 0; c < 2; ++c) { 
	int rooks_score = 0;
	int nb_rooks = pieces.get_nb_pieces(Color(c), ROOK);
	for (int i = 0; i < nb_rooks; ++i) {
	    Square s = pieces.get_position(Color(c), ROOK, i);
	    if (pawns_files[!c][s & 7]) {
		if (pawns_files[c][s & 7]) rooks_score += BONUS_ROOK_OPEN_FILE;
		else rooks_score += BONUS_ROOK_SEMI_OPEN_FILE;
	    }
	}
	if (Color(c) != player) rooks_score *= -1;
	score += rooks_score;
    }
    return score; 
}

int Game::eval() {
    int score = 0;
    Color player = current_node().get_turn_color();
    Color opponent = Color(!player);
    
    // Simple material score calculation 
    for (int c = 0; c < 2; ++c) { 
	int material_score = 0;
	int nb_pawns = 0;
	for (PieceType t = PAWN; t <= KING; t = PieceType(t + 1)) {
	    int nb_pieces = pieces.get_nb_pieces(Color(c), t);
	    material_score += PIECE_VALUE[t] * nb_pieces;
	    for (int i = 0; i < nb_pieces; ++i) {
		material_score += piece_eval(Color(c), t, i);
	    }
	    switch (t) {
		case PAWN:
		    nb_pawns = nb_pieces;
		    if (nb_pawns == 0) material_score += MALUS_NO_PAWN;
		    break;
		case KNIGHT:
		    material_score += KNIGHT_ADJ[nb_pawns] * nb_pieces;
		    break;
		case BISHOP:
		    // FIXME check if they are on different colors
		    if (nb_pieces == 2) material_score += BONUS_BISHOP_PAIR;
		    break;
		case ROOK:
		    material_score += ROOK_ADJ[nb_pawns] * nb_pieces;
		    break;
		default:
		    break;
	    } 
	}
	if (Color(c) == opponent) material_score *= -1;
	score += material_score;

	// Castling bonus/malus
	int castle_score = 0;
	if (current_node().has_castle(Color(c))) {
	    castle_score += BONUS_CASTLE;
	}
	else { // If color has broken castle,
	    // Add malus for each side where he can no more castle
	    if (!current_node().can_castle(Color(c), QUEEN)) {
		castle_score += MALUS_BREAKING_CASTLE;
	    }
	    if (!current_node().can_castle(Color(c), KING)) {
		castle_score += MALUS_BREAKING_CASTLE;
	    }
	}
	if (Color(c) == opponent) castle_score *= -1;
	score += castle_score;
    }

    score += pawn_structure_eval(pieces, player);

    return score;
}
