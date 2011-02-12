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
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>


#include "game.h"
#include "eval.h"
#include "hashtable.h"

using namespace std;

void Game::init_eval() {
    for (int i = 0; i < 64; ++i) {
	for (PieceType t = PAWN; t <= KING; t = PieceType(t + 1)) {
	    Square s = board.square_from_index(i);
	    
	    int opening_score = 0;
	    int ending_score = 0;

	    switch (t) {
		case PAWN: 
		    // Develop central pawns
		    // But not side pawns
		    opening_score = PAWN_FILES_VALUES[board.get_file(s)];

		    // Run for promotion
		    ending_score = 10 * board.get_rank(s);
		    break;
		case KNIGHT:
		case BISHOP:
		    // Develop toward center files
		    opening_score = CENTER_BONUS[board.get_file(s)];
		    if (board.is_border(s)) {
			opening_score = 2 * BORDER_MALUS;
		    }
		default:
		    ending_score = CENTER_BONUS[board.get_file(s)];
		    ending_score += CENTER_BONUS[board.get_rank(s)];
		    break;
	    }

	    // Rank bonus
	    int bonus = OPENING_RANKS_BONUS[t][board.get_rank(s)];
	    opening_score += (opening_score * bonus) / 2;

	    PST[OPENING][WHITE][t][s] = opening_score;
	    PST[ENDING][WHITE][t][s] = ending_score;
	}
    }
    // Special corrections
    // Urge to develop light pieces during oppening
    PST[OPENING][WHITE][KNIGHT][B1] = -20;
    PST[OPENING][WHITE][KNIGHT][G1] = -20;
    PST[OPENING][WHITE][BISHOP][C1] = -15;
    PST[OPENING][WHITE][BISHOP][F1] = -15;
    // But others should stay where they are
    PST[OPENING][WHITE][ROOK][A1]   = 5;
    PST[OPENING][WHITE][ROOK][H1]   = 5;
    PST[OPENING][WHITE][KING][E1]   = 5;
    // Fianchetto
    PST[OPENING][WHITE][BISHOP][B2] = 3;
    PST[OPENING][WHITE][BISHOP][G2] = 3;
    // Protection against bishop attacks
    PST[OPENING][WHITE][PAWN][A3] += 3; 
    PST[OPENING][WHITE][PAWN][H3] += 3;

    // Flip scores according to black's side
    for (int i = 0; i < 2; ++i) {
	for (int j = 0; j < 64; ++j) {
	    for (PieceType t = PAWN; t <= KING; t = PieceType(t + 1)) {
		Square ws = board.square_from_index(j);
		Square bs = board.flip(ws);
		PST[i][BLACK][t][bs] = PST[i][WHITE][t][ws];
	    }
	}
    }
    
    // Print PST
    /*
    cout << endl << "Opening pawn PST";
    string squares[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i) {
	Square s = Square(i);
	ostringstream stream;
	stream << setw(4) << PST[OPENING][WHITE][PAWN][s];
	squares[i] = stream.str();
    }
    cout << endl << board.to_string(squares); 

    cout << endl << "Ending pawn PST";
    for (int i = 0; i < BOARD_SIZE; ++i) {
	Square s = Square(i);
	ostringstream stream;
	stream << setw(4) << PST[ENDING][WHITE][PAWN][s];
	squares[i] = stream.str();
    }
    cout << endl << board.to_string(squares); 
    
    cout << endl << "Opening knight PST";
    for (int i = 0; i < BOARD_SIZE; ++i) {
	Square s = Square(i);
	ostringstream stream;
	stream << setw(4) << PST[OPENING][WHITE][KNIGHT][s];
	squares[i] = stream.str();
    }
    cout << endl << board.to_string(squares); 
    */
}

static const int LAZY_EVAL_MARGIN = PIECE_VALUE[ROOK];

int Game::eval(int alpha, int beta) {
    // Material evaluation
    //Color c = current_node().get_turn_color();
    //cout << "Eval(" << (c == WHITE ? "White" : "Black") << "): ";
    int score = material_eval();
    //cout << "material: " << score;
    //cout << "Material eval: " << score << endl;

    /*if (score == 0) return 0; // Draw
    else*/ if (score > PIECE_VALUE[KING]) return INF; // Win
    else if (score < -PIECE_VALUE[KING]) return -INF; // Loss
   
    // Lazy evaluation
    if (score + LAZY_EVAL_MARGIN < alpha) return score;
    if (score - LAZY_EVAL_MARGIN > beta) return score;

    // TODO Positionnal evaluation
    score += position_eval();
    //cout << ", position:" << score << endl;

    // TODO Mobility evaluation
    //score += mobility_eval();
    
    return score;
}

int Game::material_eval() {
    Color c;
    int score = 0;
    Node& pos = current_node();
    
    // Lookup in hash table
    bool is_empty = true;
    int hash_score = material_table.lookup(pos.material_hash(), is_empty);
    //cout << "Material_Table(" << hex << pos.material_hash() << "): ";
    if (!is_empty) {
	c = pos.get_turn_color();
	//cout << "hit " << dec << hash_score << endl;
	//return (c == WHITE ? hash_score : -hash_score);
    }
    //else cout << "miss" << endl;
    
    int material_score[2] = { 0 };
    int material_bonus[2] = { 0 };
    for (int i = 0; i < 2; ++i) {
	c = Color(i);
	int nb_pawns = 0;
	int nb_minors = 0;
	for (PieceType t = PAWN; t <= KING; t = PieceType(t + 1)) {
	    int n = pieces.get_nb_pieces(c, t);
	    // Basic values
	    material_score[c] += n * PIECE_VALUE[t]; 
	    // Bonus values
	    int adj;
	    switch (t) {
		case PAWN:
		    nb_pawns = n;
		    if (n == 0) material_bonus[c] += NO_PAWNS_MALUS;
		    break;
		case KNIGHT:
		    nb_minors = n;
		    if (n > 1) material_bonus[c] += REDUNDANCY_MALUS;
		    
		    // Value adjusted by the number of pawns on the board
		    adj = PAWNS_ADJUSTEMENT[KNIGHT][nb_pawns];
		    material_bonus[c] += n * adj;
		    break;
		case BISHOP:
		    nb_minors += n;
		    // Bishop bonus pair (from +40 to +64):
		    // less than half a pawn when most or all the pawns are on
		    // the board, and more than half a pawn when half or more 
		    // of the pawns are gone. (Kaufman 1999)
		    //
		    // No bonus for two bishops controlling the same color
		    // No bonus for more than two bishops
		    if (n == 2 && !board.is_same_color(
			    pieces.get_position(c, t, 0),
			    pieces.get_position(c, t, 1))) {
			        material_bonus[c] += BISHOP_PAIR_BONUS + 
						     (3 * 8 - nb_pawns);
		    }

		    // Value adjusted by the number of pawns on the board
		    adj = PAWNS_ADJUSTEMENT[BISHOP][nb_pawns];
		    material_bonus[c] += n * adj;
		    break;
		case ROOK:
		    // Principle of the redundancy (Kaufman 1999)
		    if (n > 1) material_bonus[c] += REDUNDANCY_MALUS;
		    
		    // Value adjusted by the number of pawns on the board
		    adj = PAWNS_ADJUSTEMENT[ROOK][nb_pawns];
		    material_bonus[c] += n * adj;
		    break;
		case QUEEN:
		    if (nb_minors > 1) {
			// With two or more minor pieces, the queen
			// equal two rooks. (Kaufman 1999)
			material_bonus[c] += (2 * PIECE_VALUE[ROOK]) -
					     (PIECE_VALUE[QUEEN]);
		    }
		    // Value adjusted by the number of pawns on the board
		    adj = PAWNS_ADJUSTEMENT[QUEEN][nb_pawns];
		    material_bonus[c] += n * adj;
		    break;
		default:
		    break;
	    }
	}
    }
    
    /*
    cout << " mat(w): " << material_score[WHITE]; 
    cout << " mat(b): " << material_score[BLACK]; 
    cout << " bon(w): " << material_bonus[WHITE]; 
    cout << " bon(b): " << material_bonus[BLACK]; 
    */

    // Insufficiant material
    for (int i = 0; i < 2; ++i) {
	c = Color(i);
	// FIDE rules for drawn
	const int K = PIECE_VALUE[KING];
	const int P = PIECE_VALUE[PAWN];
	const int N = PIECE_VALUE[KNIGHT];
	const int B = PIECE_VALUE[BISHOP];
	if (material_score[c] == PIECE_VALUE[KING]) {
	    if ((material_score[!c] == K) ||
		(material_score[!c] == K + B) ||
		(material_score[!c] == K + N) ||
		(material_score[!c] == K + N + N)) {
		    goto return_material_score;
	    }

	    // Duplicate with MALUS_NO_PAWNS ?
	    if (pieces.get_nb_pieces(Color(!c), PAWN) == 0 &&
		material_score[!c] < K + 4 * P) {
		    goto return_material_score;
	    }
	}
    }

    c = pos.get_turn_color();
    score = material_score[c] - material_score[Color(!c)];
    score += material_bonus[c] - material_bonus[Color(!c)];

    return_material_score:
	/*
	if (hash_score != score) {
	    cout << "Material_Table(" << hex << pos.material_hash() << "): ";
	    cout << "replace " << dec << hash_score << " by " << score << endl;
	}
	*/
	hash_score = (c == WHITE ? score : -score);
	material_table.save(pos.material_hash(), hash_score);
	return score;
}

int castling_score(const Node& pos, Color c) {
    int score = 0;
    if (pos.has_castle(c)) {
	score += CASTLE_BONUS;
    }
    else {
	for (PieceType t = QUEEN; t <= KING; t = PieceType(t + 1)) {
	    if (!pos.can_castle(c, t)) { // For each side where no castling
		score += BREAKING_CASTLE_MALUS; // is possible, add a malus
	    }
	}
    }
    return score;
}

int Game::position_eval() {
    int score;
    int phase = 0;
    int position_score[2][2] = { { 0 } };
    int pawns_files[2][8] = { { 0 } };
    const Node& pos = current_node();
    Color c;
    for (int i = 0; i < 2; ++i) {
	c = Color(i);
	for (PieceType t = PAWN; t <= KING; t = PieceType(t + 1)) {
	    int n = pieces.get_nb_pieces(c, t);
	    phase += n * PHASE_COEF[t];
	    for (int j = 0; j < n; ++j) {
		Square s = pieces.get_position(c, t, j);
		position_score[OPENING][c] += PST[OPENING][c][t][s];
		position_score[ENDING][c] += PST[ENDING][c][t][s];
		if (t == PAWN) pawns_files[c][board.get_file(s)]++;
	    }
	}

	int pawns_score = 0;
	for (int j = 0; j < 8; ++j) {
	    pawns_score += MULTI_PAWNS_MALUS[pawns_files[c][j]];
	}
	position_score[OPENING][c] += pawns_score;
	
	// Rooks' files bonus
	int rooks_score = 0;
	int nb_rooks = pieces.get_nb_pieces(c, ROOK);
	for (int j = 0; j < nb_rooks; ++j) {
	    Square s = pieces.get_position(c, ROOK, j);
	    if (pawns_files[!c][board.get_file(s)]) {
		if (pawns_files[c][board.get_file(s)]) {
		    rooks_score += OPEN_FILE_BONUS;
		}
		else rooks_score += HALF_OPEN_FILE_BONUS;
	    }
	}
	position_score[OPENING][c] += rooks_score;

	// Castling bonus/malus
	position_score[OPENING][c] += castling_score(pos, c);

    }
    c = pos.get_turn_color();
    //cout << " (" << (c == WHITE ? "White" : "Black") << ")";
    //cout << " w:" << position_score[OPENING][WHITE];
    //cout << " b:" << position_score[OPENING][BLACK];
    int opening, ending; // Score based on opening and ending rules
    opening = position_score[OPENING][c] - position_score[OPENING][Color(!c)];
    ending = position_score[ENDING][c] - position_score[ENDING][Color(!c)];
    
    // Tapered Eval (idea from Fruit 2.1)
    int max = PHASE_MAX;
    phase = (phase > max ? max : (phase < 0 ? 0 : phase));
    score = (opening * phase + ending * (max - phase)) / max;
    //cout << " o: " << opening << " s: " << score;
    return score;
}

