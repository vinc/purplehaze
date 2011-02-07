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

#include <string>

#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "node.h"
#include "tt.h"
#include "time.h"
#include "tree.h"
#include "zobrist.h"

enum NodeType : unsigned char {PV_NODE, CUT_NODE, ALL_NODE};

class Game
{
    private:
	Zobrist zobrist;
	Move killer_moves[MAX_DEPTH][MAX_KILLERS];

    public:
	bool output_thinking;
	unsigned int nodes_count; // Used in search
	//list<Move> moves_history; // Temporary
	Pieces pieces;
	Board board;
	Tree tree;
	Time time;
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

	bool is_check(Color c) const {
	    return board.is_attacked_by(Color(!c), 
					pieces.get_position(c, KING, 0),
					pieces);
	}


	//Moves movegen(bool captures_only = false);
	void make_move(Move m);
	void undo_move(Move m);
	bool is_legal(Move m);
	
	// Search
	int perft(int depth);
	int quiescence_search(int alpha, int beta, int depth);
	//int alphabeta_search(int alpha, int beta, int depth);
	int pv_search(int alpha, int beta, int depth, NodeType node_type);
	Move root(int max_depth);

	// Killer Moves
	Move get_killer_move(int depth, int index) {
	    return killer_moves[depth][index];
	}
	void set_killer_move(int depth, Move move);
	bool is_killer_move(int depth, Move move) {
	    return (move == killer_moves[depth][0] || 
		    move == killer_moves[depth][1]);
	};
	
	// Position's evaluation
	int piece_eval(Color c, PieceType t, int i);
	int eval();

	// Output
	void print_thinking_header();
	void print_thinking(int depth, int score, Move m);
	string output_principal_variation(int depth, Move m);
	string output_move(Move m);
	string output_square(Square s) { 
	    return output_square(board.get_file(s), board.get_rank(s));
	} ;
	string output_square(File f, Rank r);
	void print_tt_stats();
	string debug_move(Move m);
};

#endif /* !GAME_H */
