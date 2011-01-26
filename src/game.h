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

#ifndef GAME_H
#define GAME_H

#include <stack>
#include <list>
#include <bitset>
#include <string>

#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "node.h"
#include "tt.h"
#include "zobrist.h"

class Game
{
    private:
	Zobrist zobrist;
	stack<Node> tree;
	bitset<7> attack_array[240];
	Direction dir_array[240];
    public:
	int nodes_count; // Used in search
	//list<Move> moves_history; // Temporary
	Pieces pieces;
	Board board;
	Transpositions tt;
	Game();
	void add_piece(Color c, PieceType t, Square s);
	void del_piece(Color c, PieceType t, int i);
	void del_piece(Piece p) {
	    del_piece(p.get_color(), p.get_type(), p.get_index());
	};
	
	void new_node();
	void del_node();
	Node& current_node() { return tree.top(); };

	void init(string fen);

	bool can_attack(PieceType t, Square from, Square to) const {
	    return bool(attack_array[0x77 + from - to][t]);
	}
	Direction get_direction_to(Square from, Square to) const {
	    return dir_array[0x77 + from - to];
	}

	bool is_attacked_by(Color c, Square s) const;
	bool is_check(Color c) const {
	    return is_attacked_by(Color(!c), pieces.get_position(c, KING, 0));
	}

	Moves movegen(bool captures_only = false);
	void make_move(Move m);
	void undo_move(Move m);
	bool is_legal_move(Move m);
	
	// Search
	int perft(int depth);
	int quiescence(int alpha, int beta, int depth);
	int search(int alpha, int beta, int depth);
	Move root(int max_depth);
	
	// Position's evaluation
	int piece_eval(Color c, PieceType t, int i);
	int eval();
};

#endif /* !GAME_H */
