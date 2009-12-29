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
#include "eval.h"
#include "attack.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

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

int eval(Board& board, Pieces& player, Pieces& opponent) {
	//int score = rand() % 100;
	int score = 0, score_pieces_player = 0, score_pieces_opponent = 0;
	int nb_pieces_player = 0, nb_pieces_opponent = 0;
	Piece* ptr_piece;
	Color color_bishop_player = UNDEF_COLOR, color_bishop_opponent = UNDEF_COLOR;
	
	for (player.iterator = player.begin(); player.iterator != player.end(); player.iterator++) {
		ptr_piece = player.get_ptr_piece();
		if (!board.is_off_the_board(ptr_piece->get_position())) {
			score_pieces_player += ptr_piece->get_value();
			++nb_pieces_player;
			if (ptr_piece->get_type() == BISHOP) {
				color_bishop_player = (board.is_dark(ptr_piece->get_position()) ? BLACK : WHITE);
			}
			score += eval_bonus_position(ptr_piece);
		}
		else if (ptr_piece->get_type() == KING) {
			// If our king is outside, we just lost the game!
			//return -INF;
		}
	}
	score += score_pieces_player;
	
	for (opponent.iterator = opponent.begin(); opponent.iterator != opponent.end(); opponent.iterator++) {
		ptr_piece = opponent.get_ptr_piece();
		if (!board.is_off_the_board(ptr_piece->get_position())) {
			score_pieces_opponent += ptr_piece->get_value();
			++nb_pieces_opponent;
			if (ptr_piece->get_type() == BISHOP) {
				color_bishop_opponent = (board.is_dark(ptr_piece->get_position()) ? BLACK : WHITE);
			}
			score -= eval_bonus_position(ptr_piece);
		}
		else if (ptr_piece->get_type() == KING) {
			// If their king is outside, we just win the game!
			//return INF;
		}
	}
	score -= score_pieces_opponent;
	
	//Immediate draws
  	if (score_pieces_player == 0 && score_pieces_opponent == 0) {
		// Both sides have a bare king
		return 0;
	}
	else if ((score_pieces_player == 0 && score_pieces_opponent < ROOK_VALUE && nb_pieces_opponent == 1)
		|| (score_pieces_opponent == 0 && score_pieces_player < ROOK_VALUE && nb_pieces_player == 1)) {
		// One side has a king and a minor piece against a bare king
		return 0;
	}
	else if ((score_pieces_player == 0 && score_pieces_opponent == 2 * KNIGHT_VALUE)
		|| (score_pieces_opponent == 0 && score_pieces_player == 2 * KNIGHT_VALUE)) {
		// One side has two knights against the bare king
		return 0;
	}
	else if (score_pieces_player == BISHOP 
		&& score_pieces_opponent == BISHOP
		&& color_bishop_player == color_bishop_opponent) {
		// Both sides have a king and a bishop, the bishops being the same color
		return 0;
	}
	
	// Castling bonus if player have castled
	if (!board.can_castle(player.get_color())) {
		score += BONUS_CASTLE;
	}   

	// Castling malus if opponent have castled
	if (!board.can_castle(opponent.get_color())) {
		score -= BONUS_CASTLE;
	}

	// Add a little random variation
	#ifdef RANDOM_EVAL
	srand(time(NULL));
	score += rand() % BONUS_RANDOM_MAX;
	#endif
	
	//if (player.get_color() == WHITE) return score;
	//else return -score;
	return score;
}

bool is_in_check(Board& board, Pieces* ptr_pieces_player) {
	Piece* ptr_king_player = ptr_pieces_player->get_ptr_king();
	Square s = ptr_king_player->get_position();
	Color c = (ptr_king_player->get_color() == WHITE) ? BLACK : WHITE;
	Pieces attackers = is_attacked_by(board, s, c);
	return (attackers.size() == 0) ? false : true;
}