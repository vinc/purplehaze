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

ostream& operator<<(ostream& out, const Move move) {
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
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b) : ptr_piece(p), from(a), to(b) {
	type = MOVE;
	ptr_captured_piece = 0;
//	Move::moves_counter++;
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, PieceType promote) : ptr_piece(p), promotion(promote), from(a), to(b) {
	type = MOVE;
	ptr_captured_piece = 0;
//	Move::moves_counter++;
	en_passant = OUT;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, Square ep) : ptr_piece(p), en_passant(ep), from(a), to(b) {
	type = MOVE;
	ptr_captured_piece = 0;
//	Move::moves_counter++;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, MoveType t) : ptr_piece(p), type(t), from(a), to(b) {
	ptr_captured_piece = 0;
//	Move::moves_counter++;
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, MoveType t, PieceType promote) : ptr_piece(p), type(t), promotion(promote), from(a), to(b) {
	ptr_captured_piece = 0;
//	Move::moves_counter++;
	en_passant = OUT;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, Piece* c) : ptr_piece(p), ptr_captured_piece(c), from(a), to(b) {
	type = (ptr_captured_piece ? CAPTURE : MOVE);
//	Move::moves_counter++;
//	if (type == CAPTURE) Move::captures_counter++;
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, Piece* c, PieceType promote) : ptr_piece(p), ptr_captured_piece(c), promotion(promote), from(a), to(b) {
	type = (ptr_captured_piece ? CAPTURE : MOVE);
//	Move::moves_counter++;
//	if (type == CAPTURE) Move::captures_counter++;
	en_passant = OUT;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, Piece* c, MoveType t) : ptr_piece(p), ptr_captured_piece(c), type(t), from(a), to(b) {
//	Move::moves_counter++;
//	if (type == CAPTURE) Move::captures_counter++;
//	else if (type == CASTLE) Move::castles_counter++;
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

Move::Move(Piece* p, Square a, Square b, Piece* c, MoveType t, PieceType promote) : ptr_piece(p), ptr_captured_piece(c), type(t), promotion(promote), from(a), to(b) {
//	Move::moves_counter++;
//	if (type == CAPTURE) Move::captures_counter++;
//	else if (type == CASTLE) Move::castles_counter++;
	en_passant = OUT;
	nb_repetitions = 0;
	score = 0;
}

Move::~Move() {
	type = UNDEF_MOVE_TYPE;
	ptr_piece = 0;
	ptr_captured_piece = 0;
	from = OUT;
	to = OUT;
	en_passant = OUT;
	promotion = UNDEF_PIECE_TYPE;
	nb_repetitions = 0;
	score = 0;
}

/*
int Move::moves_counter = 0;
int Move::captures_counter = 0;
int Move::castles_counter = 0;
*/
/*
Move& Move::operator=( Move& move) {
	return *this;
}
*/

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

/*
MoveType Move::get_type() const {
	return type;
}

Piece* Move::get_ptr_piece() const {
	return ptr_piece;
}

Piece* Move::get_ptr_captured_piece() const {
	return ptr_captured_piece;
}
*/
/*
PieceType Move::get_promotion() const {
	return promotion;
}

Square Move::get_from() const {
	return from;
}

Square Move::get_to() const {
	return to;
}
*/
string Move::get_xboard_notation() {
	string res = "";
	res += Board::square_to_string(from);
	res += Board::square_to_string(to);
	return res;
}

/*
Square Move::get_en_passant() const {
	return en_passant;
}
*/

void Move::set_en_passant(Square ep) {
	en_passant = ep;
}

void Move::set_repetitions(unsigned int r) {
	nb_repetitions = r;
}
/*
int Move::get_repetitions() const {
	return nb_repetitions;
}

int Move::get_score() const {
	return score;
}
*/
void Move::set_score(int s) {
	score = s;
}

//const char* Move::get_san_notation() const {
string Move::get_san_notation() const {
	string san = "";
	switch (ptr_piece->get_type()) {
		case KNIGHT: san.append("N"); break;
		case BISHOP: san.append("B"); break;
		case ROOK: san.append("R"); break;
		case QUEEN: san.append("Q"); break;
		case KING: san.append("K"); break;
		default: break;
	}
	stringstream str_from("");
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
	stringstream str_to("");
	str_to << static_cast<char>(97 + (to & 7)) << 1 + (to >> 4);
	san.append(str_to.str());
	switch (promotion) {
		case KNIGHT: san.append("=N"); break;
		case BISHOP: san.append("=B"); break;
		case ROOK: san.append("=R"); break;
		case QUEEN: san.append("=Q"); break;
		default: break;
	}
	//return san.c_str();
	return san;
}

/****************************************************************
 *                                                              *
 *                             Moves                            *
 *                                                              *
 ****************************************************************/


Moves::Moves() {}

//Moves::~Moves() {}
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

/*
Move* Moves::get_ptr_move() {
	// FIXME We return a pointer on a move in the list
	// Maybe this is not a very good idea...
	return (Move*) &*iterator;
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
*/
extern Board board;
void Moves::order(/*Board& board,*/ Move* ptr_best_move/*, int ply*/) {
	list<Move>::iterator it;
	for (it = moves.begin(); it != moves.end(); ++it) {
		Move* ptr_move = &*it;

		/*
		
		//cout << *ptr_move << "(" << ptr_move->get_san_notation() << ") " << ptr_move->get_score();
		//Square s = ptr_move->get_to();
		//Color c = ptr_move->get_ptr_piece()->get_color();
		int score = 0, mvvlva = 0;
		if (ptr_best_move && *ptr_best_move == *ptr_move) {
			score = SCORE_BEST_MOVE * 2; // Must be more than PxQ score
			//cout << "The best move is fixed to " << ptr_best_move->get_san_notation() << endl;
		}
		#ifdef KILLER_HEURISTIC
		else if (board.is_first_killer_move(ptr_move, board.ply)) {
			score = SCORE_NORMAL_MOVE + 2;
		}
		else if (board.is_second_killer_move(ptr_move, board.ply)) {
			score = SCORE_NORMAL_MOVE + 1;
		}
		#endif
		else if (ptr_move->get_type() == CAPTURE) {
			score = SCORE_CAPTURE_MOVE;

			//cout << "A piece with a value of " << ptr_move->get_ptr_piece()->get_value();
			//cout << " is capturing a piece with a value of " << ptr_move->get_ptr_captured_piece()->get_value() << endl;
			
			#ifdef MVV_LVA
			score = SCORE_NORMAL_MOVE + 1;
			//mvvlva = ptr_move->get_ptr_captured_piece()->get_value() / ptr_move->get_ptr_piece()->get_value();
			mvvlva = ptr_move->get_ptr_captured_piece()->get_value() - ptr_move->get_ptr_piece()->get_value();
			//mvvlva = ptr_move->get_ptr_captured_piece()->get_value() - 5 * ptr_move->get_ptr_piece()->get_value() / 10;
			// TODO find out which is the best and bug free.

			//cout << "This give us a MVV/LVA of " << mvvlva << endl;
			#endif
		}
		else {
			score = SCORE_NORMAL_MOVE;
		}

		score *= 100;

		#ifdef MVV_LVA
		score += mvvlva;
		#endif
		
		if (ptr_move->get_promotion() != UNDEF_PIECE_TYPE) {
			score += SCORE_PROMOTION_MOVE * 10;
		}

		//score = (score < 0 ? 0 : score);
		ptr_move->set_score(max(0, score));
		
		//cout << " " << ptr_move->get_score() << endl;

		*/


		bool mvv_lva = false;
		bool losing_capture_before_normal = true;
		bool is_best_move = false;
		bool is_capture = false;
		bool is_killer = false;
		
		int score;
		
		if (ptr_best_move && *ptr_best_move == *ptr_move) {
			score = SCORE_BEST_MOVE;
			is_best_move = true;
		}		
		else if (ptr_move->get_type() == CAPTURE) {
			score = SCORE_CAPTURE_MOVE;
			is_capture = true;
		}		
		else if (ptr_move->get_promotion() != UNDEF_PIECE_TYPE) {
			score = SCORE_PROMOTION_MOVE;
		}
		else {
			score = SCORE_NORMAL_MOVE;
		}

		#ifdef MVV_LVA
		mvv_lva = true;
		score = score * PAWN_VALUE + QUEEN_VALUE; // More accurate scores needed
		
		if (!is_best_move && is_capture) {
			// We do not want a negative score
			int minimum = 0;
			if (losing_capture_before_normal) {
				// If we want the losing captures before normal moves
				// But after killer moves
				minimum = SCORE_NORMAL_MOVE * PAWN_VALUE + QUEEN_VALUE + 1;
			}
			// Most Valuable Victim - Least Valuable Aggressor
			int victim = ptr_move->get_ptr_captured_piece()->get_value();
			int aggressor = ptr_move->get_ptr_piece()->get_value();
			score = max(victim - aggressor + 2 * PAWN_VALUE + QUEEN_VALUE, minimum);
		}
		#endif

		#ifdef KILLER_HEURISTIC
		
		if (!is_best_move && !is_capture && board.is_first_killer_move(ptr_move, board.ply)) {
			score = SCORE_KILLER_MOVE;
			is_killer = true;			
		}
		else if (!is_best_move && !is_capture && board.is_second_killer_move(ptr_move, board.ply)) {
			score = SCORE_KILLER_MOVE - 1;
			is_killer = true;
		}
		

		if (mvv_lva && is_killer) {
			score += SCORE_NORMAL_MOVE * PAWN_VALUE + QUEEN_VALUE;
		}
		#endif

		// Set the score of the move
		ptr_move->set_score(score);
		
	}

	// Order the moves
	moves.sort();
}

void Moves::print() {
	list<Move>::iterator it;
	for (it = moves.begin(); it != moves.end(); ++it) {
		Move* ptr_move = &*it;
		cout << ptr_move->get_san_notation() << " => ";
		cout << ptr_move->get_score();
		cout << endl;
	}
}
		