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

#include <list>

#ifndef MOVE_H
#define MOVE_H

const int NB_ROOK_MOVES = 4;
const MoveOrientation ROOK_MOVES[NB_ROOK_MOVES] = {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

const int NB_BISHOP_MOVES = 4;
const MoveOrientation BISHOP_MOVES[NB_BISHOP_MOVES] = {
	UP_RIGHT,
	DOWN_RIGHT,
	DOWN_LEFT,
	UP_LEFT
};

const int NB_KNIGHT_MOVES = 8;
const MoveOrientation KNIGHT_MOVES[NB_KNIGHT_MOVES] = {
	UP_UP_RIGHT, 
	RIGHT_UP_RIGHT, 
	RIGHT_DOWN_RIGHT, 
	DOWN_DOWN_RIGHT, 
	DOWN_DOWN_LEFT, 
	LEFT_DOWN_LEFT, 
	LEFT_UP_LEFT, 
	UP_UP_LEFT
};

const int NB_QUEEN_MOVES = 8;
const MoveOrientation QUEEN_MOVES[NB_QUEEN_MOVES] = {
	UP,
	UP_RIGHT,
	RIGHT,
	DOWN_RIGHT,
	DOWN,
	DOWN_LEFT,
	LEFT,
	UP_LEFT
};

const int NB_KING_MOVES = 8;
const MoveOrientation KING_MOVES[NB_KING_MOVES] = {
	UP,
	UP_RIGHT,
	RIGHT,
	DOWN_RIGHT,
	DOWN,
	DOWN_LEFT,
	LEFT,
	UP_LEFT
};

class Move
{
	friend ostream& operator<<(ostream& out, const Move& move);
	
	private:
		Piece* ptr_piece;
		Piece* ptr_captured_piece;
		unsigned char nb_repetitions; // From 0 to 50, used to save board.repetition
		unsigned char score;
		MoveType type;
		PieceType promotion;
		Square en_passant;
		Square from;
		Square to;
		
	public:
		/*
		static int moves_counter;
		static int captures_counter;
		static int castles_counter;
		*/
		Move();
		~Move();
		Move(Piece* p, Square a, Square b);
		Move(Piece* p, Square a, Square b, PieceType promote);
		Move(Piece* p, Square a, Square b, Square ep);
		Move(Piece* p, Square a, Square b, MoveType t);
		Move(Piece* p, Square a, Square b, MoveType t, PieceType promote);
		Move(Piece* p, Square a, Square b, Piece* c);
		Move(Piece* p, Square a, Square b, Piece* c, PieceType promote);
		Move(Piece* p, Square a, Square b, Piece* c, MoveType t);
		Move(Piece* p, Square a, Square b, Piece* c, MoveType t, PieceType promote);
		//Move& operator=( Move& move);
		bool operator<(const Move& move) const;
		bool operator==(const Move& move) const;
		bool operator!=(const Move& move) const {return (*this == move ? false : true);};		
		MoveType get_type() const {return type;};
		Piece* get_ptr_piece() const {return ptr_piece;};
		Piece* get_ptr_captured_piece() const {return ptr_captured_piece;};
		PieceType get_promotion() const {return promotion;};
		Square get_from() const {return from;};
		Square get_to() const {return to;};
		string get_xboard_notation();
		Square get_en_passant() const {return en_passant;};
		void set_en_passant(Square ep);
		void set_repetitions(unsigned char r);
		unsigned char get_repetitions() const {return nb_repetitions;};
		unsigned char get_score() const {return score;};
		void set_score(int s);
		const char* get_san_notation() const;
};

class Moves
{
	private:
		list<Move> moves;
	public:
		Moves();
		~Moves();
		//Moves(Moves& moves);
		void insert(Move& move, Position position);
		Move* get_ptr_move();
		//typedef std::set<Move>::iterator iterator;
		list<Move>::iterator iterator;
		list<Move>::iterator begin();
		list<Move>::iterator end();
		int size();
		void clear();
		void sort();
		void unique();
		void order(/*Board& board,*/ Move* ptr_best_move/*, int ply*/);
};

#endif /* !MOVE_H */
