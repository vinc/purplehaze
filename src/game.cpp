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

using namespace std;

Game::Game() {
    nodes_count = 0;
    output_thinking = false;
    tt.clear();
    material_table.clear();
    clear_killers();
    
    // Make PST
    init_eval();
    
    // Initialize MVV/LVA score array
    Moves::init_mvv_lva_scores();
}

void Game::clear_killers() {
    for (int i = 0; i < MAX_DEPTH; ++i) {
	for (int j = 0; j < MAX_KILLERS; ++j) {
	    killer_moves[i][j] = Move();
	}
    }
}


void Game::add_piece(Color c, PieceType t, Square s) {
    int i = pieces.get_nb_pieces(c, t);
    pieces.set_position(c, t, i, s);
    board.set_piece(Piece(c, t, i), s);
    pieces.inc_nb_pieces(c, t);
    
    // Update Zobrist hash
    Node& pos = current_node();
    zobrist.update_piece(pos.hash(), c, t, s);
    // For the material hash the position is irrelevant, but each piece
    // must have a unique hash. Hack using Square(i).
    //cout << "Add " << Piece(c, t, i) << " xor " << i << ", " << i + 1 << endl;
    zobrist.update_piece(pos.material_hash(), c, t, Square(i));
    zobrist.update_piece(pos.material_hash(), c, t, Square(i + 1));
}

void Game::del_piece(Color c, PieceType t, int i) {
    // Remove the piece, and put in its place the higher index piece of the
    // same color and type in order to avoid holes (idea from Mediocre Chess)
    Square emptied = pieces.get_position(c, t, i); // Get piece's position
    board.set_piece(Piece(), emptied);		   // Remove it from board
    pieces.dec_nb_pieces(c, t);			   // and from pieces list
    pieces.set_position(c, t, i, OUT);		   // TODO: not needed
    int j = pieces.get_nb_pieces(c, t);		   // Last piece's index
    if (pieces.get_nb_pieces(c, t) > 0 && i != j) {
	// Swap i and j and update board
	Square s = pieces.get_position(c, t, j);   // Last piece's position
	pieces.set_position(c, t, i, s);	   // Fill the hole left    
	pieces.set_position(c, t, j, OUT);	   // TODO: not needed
	board.set_piece(Piece(c, t, i), s);	   // Update board
    }
    // Update Zobrist hash
    Node& pos = current_node();
    zobrist.update_piece(pos.hash(), c, t, emptied);
    // For the material hash the position is irrelevant, but each piece
    // must have a unique hash. Hack using Square(j).
    //cout << "Del " << Piece(c, t, i) << " xor " << j + 1;
    zobrist.update_piece(pos.material_hash(), c, t, Square(j + 1));
    //if (j + 1 > 0) {
	//cout << ", " << j << endl;
	zobrist.update_piece(pos.material_hash(), c, t, Square(j));
    //}
    //cout << endl;
}

void Game::new_node() {
    // Take a "snapshot" of the current position
    tree.push();

    // Remove the previous en passant square from the Zobrist hash
    zobrist.update_en_passant(current_node().hash(), 
			      current_node().get_en_passant());
    
    // Update the position for a new move
    current_node().inc_ply();
    current_node().change_side();
    zobrist.change_side(current_node().hash());
}

void Game::del_node() {
    // Take back the previous "snapshot"
    tree.pop();
}

/*
 * Killer move setter for the Killer Heuristic in move ordering.
 */
void Game::set_killer_move(int depth, Move move) {
    if (move != killer_moves[depth][0]) {
	killer_moves[depth][1] = killer_moves[depth][0];
	killer_moves[depth][0] = move;
    }
}
