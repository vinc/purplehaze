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

#include <sstream>

#include <stdio.h>
#include <time.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include "common.h"
#include "piece.h"
#include "move.h"
#include "zobrist.h"
#include "board.h"
#include "attack.h"

using namespace std;

extern Board board;

ostream& operator<<(ostream& out, const Move& move) {
	#ifdef SAN_OUTPUT
	return out << move.get_san_notation();
	#else
	/*
	switch (move.get_ptr_piece()->get_type()) {
		case KNIGHT: out << 'N'; break;
		case BISHOP: out << 'B'; break;
		case ROOK: out << 'R'; break;
		case QUEEN: out << 'Q'; break;
		case KING: out << 'K'; break;
		default: break;
	}
	out << (move.get_type() == CAPTURE ? "x" : "");
	*/
	out << static_cast<char>(97 + (move.get_from() & 7)) << 1 + (move.get_from() >> 4);
	out << static_cast<char>(97 + (move.get_to() & 7)) << 1 + (move.get_to() >> 4);
	switch (move.get_promotion()) {
		case KNIGHT: out << 'n'; break;
		case BISHOP: out << 'b'; break;
		case ROOK: out << 'r'; break;
		case QUEEN: out << 'q'; break;
		default: break;
	}
    return out;
	#endif
}

Move::Move() {
	type = UNDEF_MOVE_TYPE;
	ptr_piece = 0;
	ptr_captured_piece = 0;
	from = OUT;
	to = OUT;
	Move::moves_counter++;
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b) : ptr_piece(p), from(a), to(b) {
	type = MOVE;
	ptr_captured_piece = 0;
	Move::moves_counter++;
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, PieceType promote) : ptr_piece(p), from(a), to(b), promotion(promote) {
	type = MOVE;
	ptr_captured_piece = 0;
	Move::moves_counter++;
	en_passant = OUT;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, Square ep) : ptr_piece(p), from(a), to(b), en_passant(ep) {
	type = MOVE;
	ptr_captured_piece = 0;
	Move::moves_counter++;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, MoveType t) : ptr_piece(p), from(a), to(b), type(t) {
	ptr_captured_piece = 0;
	Move::moves_counter++;
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, MoveType t, PieceType promote) : ptr_piece(p), from(a), to(b), type(t), promotion(promote) {
	ptr_captured_piece = 0;
	Move::moves_counter++;
	en_passant = OUT;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, Piece* c) : ptr_piece(p), from(a), to(b), ptr_captured_piece(c) {
	type = (ptr_captured_piece ? CAPTURE : MOVE);
	Move::moves_counter++;
	if (type == CAPTURE) Move::captures_counter++;
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, Piece* c, PieceType promote) : ptr_piece(p), from(a), to(b), ptr_captured_piece(c), promotion(promote) {
	type = (ptr_captured_piece ? CAPTURE : MOVE);
	Move::moves_counter++;
	if (type == CAPTURE) Move::captures_counter++;
	en_passant = OUT;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, Piece* c, MoveType t) : ptr_piece(p), from(a), to(b), ptr_captured_piece(c), type(t) {
	Move::moves_counter++;
	if (type == CAPTURE) Move::captures_counter++;
	else if (type == CASTLE) Move::castles_counter++;
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, Piece* c, MoveType t, PieceType promote) : ptr_piece(p), from(a), to(b), ptr_captured_piece(c), type(t), promotion(promote) {
	Move::moves_counter++;
	if (type == CAPTURE) Move::captures_counter++;
	else if (type == CASTLE) Move::castles_counter++;
	en_passant = OUT;
	nb_repetitions = 0;
	score = 0;
}

Move::~Move() {}

int Move::moves_counter = 0;
int Move::captures_counter = 0;
int Move::castles_counter = 0;

bool Move::operator<(const Move &move) const {
	/*	
	int a = 0, b = 0;
	a += 10000 * this->score;
	b += 10000 * move.score;
	a += 500 * this->promotion;
	b += 500 * move.promotion;	
	a += 100 * this->type;
	b += 100 * move.type;
	a += 2 * this->from;
	b += 2 * move.from;
	a += 1 * this->to;
	b += 1 * move.to;
	return a > b;
	*/
	return this->score > move.score;
}

bool Move::operator==(const Move &move) const {
	return this->from == move.from
		&& this->to == move.to
		&& this->promotion == move.promotion;
}

MoveType Move::get_type() const {
	return type;
}

Piece* Move::get_ptr_piece() const {
	return ptr_piece;
}

Piece* Move::get_ptr_captured_piece() const {
	return ptr_captured_piece;
}

PieceType Move::get_promotion() const {
	return promotion;
}

Square Move::get_from() const {
	return from;
}

Square Move::get_to() const {
	return to;
}

string Move::get_xboard_notation() {
	string res = "";
	res += Board::square_to_string(from);
	res += Board::square_to_string(to);
	return res;
}


Square Move::get_en_passant() const {
	return en_passant;
}

void Move::set_en_passant(Square ep) {
	en_passant = ep;
}

void Move::set_repetitions(int r) {
	nb_repetitions = r;
}

int Move::get_repetitions() const {
	return nb_repetitions;
}

int Move::get_score() const {
	return score;
}

void Move::set_score(int s) {
	score = s;
}

const char* Move::get_san_notation() const {
	string san;
	switch (ptr_piece->get_type()) {
		case KNIGHT: san.append("N"); break;
		case BISHOP: san.append("B"); break;
		case ROOK: san.append("R"); break;
		case QUEEN: san.append("Q"); break;
		case KING: san.append("K"); break;
		default: break;
	}
	stringstream str_from;
	Pieces attackers = is_attacked_by(board, to, ptr_piece->get_color());
	switch (type) {
		case CAPTURE:
			if (ptr_piece->get_type() == PAWN) {
				str_from << static_cast<char>(97 + (from & 7));
			}
			else if (attackers.size() > 1) {
				for (attackers.iterator = attackers.begin(); attackers.iterator != attackers.end(); ++attackers.iterator) {
					Piece* ptr_attacker = attackers.get_ptr_piece();
					if (ptr_attacker->get_position() != ptr_piece->get_position()
						&& ptr_attacker->get_type() == ptr_piece->get_type()) {
							str_from << static_cast<char>(97 + (from & 7));
					}
				}
			}
			san.append(str_from.str());
			san.append("x");
			break;
		case CASTLE:
			if (board.get_file(ptr_captured_piece->get_position()) == A_FILE) {
				return "O-O-O";
			}
			else {
				return "O-O";
			}
			break;
		default:
			break;
	}
	stringstream str_to;
	str_to << static_cast<char>(97 + (to & 7)) << 1 + (to >> 4);
	san.append(str_to.str());
	switch (promotion) {
		case KNIGHT: san.append("=N"); break;
		case BISHOP: san.append("=B"); break;
		case ROOK: san.append("=R"); break;
		case QUEEN: san.append("=Q"); break;
		default: break;
	}
	return san.c_str();
}

/****************************************************************
 *                                                              *
 *                             Moves                            *
 *                                                              *
 ****************************************************************/


Moves::Moves() {}

Moves::~Moves() {}
/*
Moves::Moves(Moves& moves) {
	
}
*/

void Moves::insert(Move& move, Position position) {
	//moves.insert(move);
	switch (position) {
		case FRONT: moves.push_front(move); break;
		case BACK: moves.push_back(move); break;
		default: moves.push_back(move); break;
	}
}

Move* Moves::get_ptr_move() {
	//Move* ptr_move = new Move(iterator->get_ptr_piece(), iterator->get_from(), iterator->get_to(), iterator->get_ptr_captured_piece());
	Move* ptr_move = &*iterator;
	return ptr_move;
}

list<Move>::iterator Moves::begin() {
	return moves.begin();
}

list<Move>::iterator Moves::end() {
	return moves.end();
}

int Moves::size() {
	return moves.size();
}

void Moves::clear() {
	moves.clear();
}

void Moves::sort() {
	moves.sort();
}

void Moves::unique() {
	moves.unique();
}

void Moves::order(/*Board& board, */Move* ptr_best_move) {
	list<Move>::iterator it;
	for (it = moves.begin(); it != moves.end(); ++it) {
		Move* ptr_move = &*it;
		//cout << *ptr_move << "(" << ptr_move->get_san_notation() << ") " << ptr_move->get_score();
		//Square s = ptr_move->get_to();
		//Color c = ptr_move->get_ptr_piece()->get_color();

		if (ptr_best_move && *ptr_best_move == *ptr_move) {
			// Max score for the best move
			ptr_move->set_score(SCORE_BEST_MOVE);
			//cout << *ptr_move << " is the best move according to order()" << endl;
		}
		else if (ptr_move->get_type() == CAPTURE) {
			/*if (-static_exchange_evaluation(board, s, c) > 0) {
				ptr_move->set_value(SCORE_KILLER_MOVE);
			}
			else {*/
				ptr_move->set_score(SCORE_CAPTURE_MOVE);
			//}
		}
		else {
			ptr_move->set_score(SCORE_NORMAL_MOVE);
		}

		if (ptr_move->get_promotion() != UNDEF_PIECE_TYPE) {
			ptr_move->set_score(ptr_move->get_score() + SCORE_PROMOTION_MOVE);
		}
		//cout << " " << ptr_move->get_score() << endl;
	}
	moves.sort();
}

