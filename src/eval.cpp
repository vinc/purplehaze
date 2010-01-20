/*  PurpleHaze 1.0
    Copyright (C) 2007-2009  Vincent Ollivier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "piece.h"
#include "move.h"
#include "zobrist.h"
#include "board.h"
#include "movegen.h"
#include "search.h"
#include "attack.h"
#include "eval.h"


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <vector>

int static_exchange_evaluation(Board board, Square square, Color opponent_color) {
	Color player_color = (opponent_color == WHITE) ? BLACK : WHITE;
	int value = 0;
	Pieces attackers = is_attacked_by(board, square, opponent_color);
	
	// If the square is attacked on this side we go on
	if (attackers.size()) {

		attackers.sort();
		
		Piece* ptr_attacker = attackers.get_ptr_smallest();
		Piece* ptr_captured_piece = board.get_ptr_piece(square); //TODO wrong piece returned
		if (ptr_attacker && ptr_captured_piece) {
			Move capture(ptr_attacker, ptr_attacker->get_position(), square, ptr_captured_piece);
			make_move(board, capture);
			/*
			see_value = -static_exchange_evaluation(board, square, player_color);
			if (see_value >= 0) {
				// Do not make captures if they lose material
				value = ptr_captured_piece->get_value() + see_value;
			}
			*/
			value = max(0, ptr_captured_piece->get_value() - static_exchange_evaluation(board, square, player_color));
			unmake_move(board, capture);
		}
	}
	return value;
}

int eval_bonus_position(Piece* ptr_piece) {
	int score = 0;
	switch (ptr_piece->get_color()) {
		case WHITE:
			switch (ptr_piece->get_type()) {
				case PAWN:
					score += PAWN_PCSQ[ptr_piece->get_position()];
					break;
				case KNIGHT:
					score += KNIGHT_PCSQ[ptr_piece->get_position()];
					break;
				case BISHOP:
					score += BISHOP_PCSQ[ptr_piece->get_position()];
					break;
				case ROOK:
					score += BISHOP_PCSQ[ptr_piece->get_position()];
					break;
				case QUEEN:
					score += BISHOP_PCSQ[ptr_piece->get_position()];
					break;
				case KING:
					score += BISHOP_PCSQ[ptr_piece->get_position()];
					break;
				default:
					break;
			}
			break;
		case BLACK:
			switch (ptr_piece->get_type()) {
				case PAWN:
					score += PAWN_PCSQ[FLIP[ptr_piece->get_position()]];
					break;
				case KNIGHT:
					score += KNIGHT_PCSQ[FLIP[ptr_piece->get_position()]];
					break;
				case BISHOP:
					score += BISHOP_PCSQ[FLIP[ptr_piece->get_position()]];
					break;
				case ROOK:
					score += BISHOP_PCSQ[FLIP[ptr_piece->get_position()]];
					break;
				case QUEEN:
					score += BISHOP_PCSQ[FLIP[ptr_piece->get_position()]];
					break;
				case KING:
					score += BISHOP_PCSQ[FLIP[ptr_piece->get_position()]];
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	return score;
}

/** A pawn is isolated if there is no pawn of the same color
  * on the neighboor files on his back */
bool is_isolated_pawn(Board& board, Piece* ptr_piece) {	
	Color color = ptr_piece->get_color();
	MoveOrientation direction = (color == WHITE ? DOWN : UP);
	for (Square position = ptr_piece->get_position(); !board.is_off_the_board(position); position = Square(position + direction)) {
		Piece* left_neighboor = board.get_ptr_piece(Square(position + LEFT));
		Piece* right_neighboor = board.get_ptr_piece(Square(position + RIGHT));
		if (left_neighboor && left_neighboor->get_type() == PAWN && left_neighboor->get_color() == color) {
			return false;
		}
		else if (right_neighboor && right_neighboor->get_type() == PAWN && right_neighboor->get_color() == color) {
			return false;
		}
	}
	return true;
}

/** A pawn is passed if there is no pawn of the opposite color
  * in front of him nor on the neighboor files */
bool is_passed_pawn(Board& board, Piece* ptr_piece) {	
	Color color = ptr_piece->get_color();
	MoveOrientation direction = (color == WHITE ? UP : DOWN);
	for (Square position = Square(direction + ptr_piece->get_position()); !board.is_off_the_board(position); position = Square(position + direction)) {
		Piece* front_neighboor = board.get_ptr_piece(position);
		Piece* left_neighboor = board.get_ptr_piece(Square(position + LEFT));
		Piece* right_neighboor = board.get_ptr_piece(Square(position + RIGHT));
		if (front_neighboor && front_neighboor->get_type() == PAWN && front_neighboor->get_color() != color) {
			return false;
		}
		else if (left_neighboor && left_neighboor->get_type() == PAWN && left_neighboor->get_color() != color) {
			return false;
		}
		else if (right_neighboor && right_neighboor->get_type() == PAWN && right_neighboor->get_color() != color) {
			return false;
		}
	}
	return true;
}

bool is_in_check(Board& board, Pieces* ptr_pieces_player) {
	Piece* ptr_king_player = ptr_pieces_player->get_ptr_king();
	Square s = ptr_king_player->get_position();
	Color c = (ptr_king_player->get_color() == WHITE) ? BLACK : WHITE;
	return is_attacked(board, s, c);
}

// Evaluation of material and piece-square tables
int Evaluation::get_lazy_eval() {
	return lazy_score;
}
	
void Evaluation::pre_build(Board& board) {	
	assert(lazy_score == 0);
	assert(material_score == 0);
	assert(positional_score == 0);
	
	int ply = (board.positions_history.size() + 1) / 2;
	const int END_OF_OPENING = 8;
	
	for (pieces.iterator = pieces.begin(); pieces.iterator != pieces.end(); pieces.iterator++) {
		Piece* ptr_piece = pieces.get_ptr_piece();

		if (!board.is_off_the_board(ptr_piece->get_position())) {
			// Add material value and bonus position
			if (ptr_piece->get_type() != KING) {
				material_score += ptr_piece->get_value();
			}
			positional_score += eval_bonus_position(ptr_piece);
		
			// Penality for wrong early moves
			if (ply < END_OF_OPENING) {
				switch (ptr_piece->get_type()) {
					case KING:
						if (!board.have_castled(color) && ptr_piece->get_nb_moves() > 0) {
							lazy_score += MALUS_KING_BREAKING_CASTLE_RIGHT;
						}
						break;
					case QUEEN:
						if (ptr_piece->get_nb_moves() > 0) {
							lazy_score += (END_OF_OPENING - ptr_piece->get_nb_moves()) * MALUS_QUEEN_EARLY_MOVE;
						}
						break;
					case BISHOP:
						//TODO Should be the case color and not the piece color?
						color_single_bishop = ptr_piece->get_color();
						break;
					default:
						break;
				}
			}
		}
	}
	lazy_score += material_score + positional_score;

	// Extra bonus if player have castled
	if (board.have_castled(color)) {
		//board.print();
		//cout << (color == WHITE ? "White" : "Black") << " side have castled!" << endl;
		lazy_score += BONUS_CASTLE;
	}
}

Evaluation::Evaluation(Pieces& p) : pieces(p) {
	color = p.get_color();
	lazy_score = 0;
	material_score = 0;
	positional_score = 0;
	for (int i = 0; i < NB_TYPE; ++i) {
		for (int j = 0; j < NB_FILE; ++j) {
			nb_pieces[i] = 0;
			nb_pieces_file[i][j] = 0;
			nb_pieces_rank[i][j] = 0;
		}
	}
}

// Building pieces statistics for better evaluation
void Evaluation::build(Board& board) {
	for (pieces.iterator = pieces.begin(); pieces.iterator != pieces.end(); pieces.iterator++) {
		Piece* ptr_piece = pieces.get_ptr_piece();
		PieceType type = ptr_piece->get_type();
		++nb_pieces[type];
		if (type == ROOK || type == PAWN) {
			Square position = ptr_piece->get_position();
			++nb_pieces_file[type][board.get_file(position)];
			++nb_pieces_rank[type][board.get_rank(position)];
		}
		else if (type == PAWN) {
			pawns.push_back(ptr_piece);
		}
	}
}

int Evaluation::king_eval(Board& board) {
	int score = 0;
	// King Safety in the middlegame
	// Centralization in the endgame
	// Penalty for standing on a wing with no pawns present in the endgame
	// Pins/xrays
	// Castling Rights [Done in lazy eval]
	return score;
}

int Evaluation::queens_eval(Board& board) {
	int score = 0;
	// Penality for early developpement [Done in lazy eval]
	return score;
}
int Evaluation::rooks_eval(Board& board, Evaluation opponent) {
	int score = 0;
	// Increasing value as pawns disappear
	score += ROOK_ADJ[nb_pieces[PAWN]] * nb_pieces[ROOK];
	// Rook on open file
	for (int file = 0; file < 8; ++file) {
		if (nb_pieces_file[ROOK][file] && !nb_pieces_file[PAWN][file] && !opponent.nb_pieces_file[PAWN][file]) {
			score += BONUS_ROOK_OPEN_FILE * nb_pieces_file[ROOK][file];
		}
	}
	// Rook on seventh (possibly also eigth) rank
	// The PCSQ could be enought for that case
	int rank = (color == WHITE ? 1 : 6);
	if (opponent.nb_pieces_rank[PAWN][rank] > 0) {
		score += BONUS_ROOK_SEVENTH_RANK * nb_pieces_rank[ROOK][rank];
	}
	// Rook behind Passed Pawn
	// Penalty for a Rook blocked by an uncastled King
	// Rook on the same rank?
	for (int rank = 0; rank < 8; ++rank) {
		score += nb_pieces_rank[ROOK][rank];
	}
	return score;
}
int Evaluation::bishops_eval(Board& board) {
	int score = 0;
	// Bishop pair
	if (nb_pieces[BISHOP] > 1) score += BONUS_BISHOP_PAIR;
	// Bad Bishop
	// Color Weakness
	// Fianchetto
	// Returning Bishop
	// Bishop trapped by enemy pawns on A2/H2/A7/H7 or on A3/H3/A6/H6
	return score;
}
int Evaluation::knights_eval(Board& board) {
	int score = 0;
	// Decreasing value as pawns disappear
	score += KNIGHT_ADJ[nb_pieces[PAWN]] * nb_pieces[KNIGHT];
	// Outposts
	// Knight trapped on A8/H8/A7/H7 or A1/H1/A2/H2
	// Penalty for blocking a "c" pawn in closed openings 
	// (Crafty defines it as follows: white knight on c3, white pawns on c2 and d4, no white pawn on e4)
	// When calculating knight mobility, it is advisable to omit squares controlled by enemy pawns
	return score;
}
int Evaluation::pawns_eval(Board& board, Evaluation opponent) {
	int score = 0;
	// Pawn Structure
	for (int i = 0; i < (int) pawns.size(); ++i) {
		// Penality for isolated pawn
		if (is_isolated_pawn(board, pawns[i])) {
			Square position = pawns[i]->get_position();
			int bonus = (color == WHITE ? WEAK_PAWN_PCSQ[position] : WEAK_PAWN_PCSQ[FLIP[position]]);
			// If the file is half-open, the penality increase
			if (opponent.nb_pieces_file[PAWN][board.get_file(position)] > 0) {
				bonus *= 3;
			}
			score += bonus;		
		}
		// Bonus for passed pawn
		if (is_passed_pawn(board, pawns[i])) {
			Square position = pawns[i]->get_position();
			score += (color == WHITE ? PASSED_PAWN_PCSQ[position] : PASSED_PAWN_PCSQ[FLIP[position]]);	
		}
	}
	// Penality for more than two pawns by file
	for (int file = 0; file < 8; ++file) {
		score += MALUS_MULTI_PAWN[nb_pieces_file[PAWN][file]];
	}
	// Malus for having no pawn
	if (nb_pieces[PAWN] == 0) {
		score += MALUS_NO_PAWN;
	}
	// Pawn Center
	// penalty for "d" and "e" pawns blocked on their initial squares
	return score;
}

bool is_draw(Board& board, Evaluation player, Evaluation opponent) {
  	if (player.material_score == 0 && opponent.material_score == 0) {
		// Both sides have a bare king
		return true;
	}
	else if ((player.material_score == 0 && opponent.material_score < ROOK_VALUE && (opponent.nb_pieces[BISHOP] > 0 || opponent.nb_pieces[KNIGHT] > 0) && opponent.nb_pieces[PAWN] == 0)
		|| (opponent.material_score == 0 && player.material_score < ROOK_VALUE && (player.nb_pieces[BISHOP] > 0 || player.nb_pieces[KNIGHT] > 0) && player.nb_pieces[PAWN] == 0)) {
		// One side has a king and a minor piece against a bare king
		return true;
	}
	else if ((player.material_score == 0 && opponent.material_score == 2 * KNIGHT_VALUE)
		|| (opponent.material_score == 0 && player.material_score == 2 * KNIGHT_VALUE)) {
		// One side has two knights against the bare king
		return true;
	}
	else if (player.material_score == BISHOP 
		&& opponent.material_score == BISHOP
		&& player.color_single_bishop == opponent.color_single_bishop) {
		// Both sides have a king and a bishop, the bishops being the same color
		return true;
	}
	else {
		return false;
	}
}

int fast_eval(Board& board, Pieces& player_pieces, Pieces& opponent_pieces) {
	int score = 0;

	// Build stats for lazy eval
	Evaluation player(player_pieces), opponent(opponent_pieces);
	player.pre_build(board);
	opponent.pre_build(board);
	
	// Lazy eval
	score += player.get_lazy_eval();
	score -= opponent.get_lazy_eval();

	return score;
}

// Main
int eval(Board& board, Pieces& player_pieces, Pieces& opponent_pieces) {
	int score = 0;

	// Build stats for lazy eval
	Evaluation player(player_pieces), opponent(opponent_pieces);
	player.pre_build(board);
	opponent.pre_build(board);
	
	// Lazy eval
	score += player.get_lazy_eval();
	score -= opponent.get_lazy_eval();

	//bool display = board.have_castled(player.color) || board.have_castled(opponent.color);
	//bool display = board.have_castled(player.color) && player.color == WHITE;
	bool display = false;
	if (display) board.print();
	
	if (display) cout << "Score from " << (player.color == WHITE ? "white" : "black") << " point of view:" << endl;
	
	if (display) cout << "Lazy eval: " << score << endl;
	
	//TODO replace 100 by alpha
	///*if (score > 100)*/ return score;
	
	// Build stats	
	player.build(board);
	opponent.build(board);
	
	// Draw eval
	if (is_draw(board, player, opponent)) return 0;
	
	// Pieces eval
	score += player.king_eval(board);
	score -= opponent.king_eval(board);
	
	score += player.queens_eval(board);
	score -= opponent.queens_eval(board);
	
	score += player.rooks_eval(board, opponent);
	score -= opponent.rooks_eval(board, player);
	
	score += player.bishops_eval(board);
	score -= opponent.bishops_eval(board);
	
	score += player.knights_eval(board);
	score -= opponent.knights_eval(board);
	
	score += player.pawns_eval(board, opponent);
	score -= opponent.pawns_eval(board, player);

	if (display) cout << "Eval: " << score << endl;
	
	return score;
}


bool is_promoting_pawn(Board& board) {
	Square s;
	Color c = board.get_turn_color();
	switch (c) {
		case WHITE:
			s = A7;
			break;
		case BLACK:
			s = A2;
			break;
		default:
			s = OUT;
			break;
	}	
	while (!board.is_off_the_board(s)) {
		Piece* ptr_piece = board.get_ptr_piece(s);
		if (ptr_piece && ptr_piece->get_color() == c && ptr_piece->get_type() == PAWN) {
			return true;
		}
		s = board.get_square_right(s);
	}
	return false;
}
