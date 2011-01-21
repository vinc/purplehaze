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
#include <string>

#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "node.h"

class Game
{
    private:
	stack<Node> tree;
    public:
	Pieces pieces;
	Board board;
	Game();
	void add_piece(Color c, PieceType t, Square s);
	void del_piece(Piece p) {
	    del_piece(p.get_color(), p.get_type(), p.get_index());
	};
	void del_piece(Color c, PieceType t, int i);
	
	void new_node();
	void del_node();
	Node& current_node() { return tree.top(); };

	void init(string fen);

	Moves movegen(bool captures_only = false);
	void make_move(Move m);
	void undo_move(Move m);
	
	int perft(int depth);
};

#endif /* !GAME_H */
