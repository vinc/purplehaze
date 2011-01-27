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
    
    // Initialize direction array
    for (int i = 0; i < 240; ++i) { 
	dir_array[i] = NO_DIR;
    }

    // Initialize attack array
    for (int i = 0; i < 64; ++i) { 
	for (int j = 0; j < 64; ++j) { 
	    Square from = Square(i + (i & ~7));
	    Square to = Square(j + (j & ~7));
	    int diff = 0x77 + from - to;
	    for (PieceType t = KNIGHT; t <= KING; t = PieceType(t + 1)) {
		const Direction * dirs = PIECES_DIRS[t];
		for (int i = 0; i < NB_DIRS[t]; ++i) {
		    Square s = Square(from + dirs[i]);
		    while (!board.is_out(s)) {
			if (s == to) {
			    attack_array[diff].set(t, true);
			    dir_array[diff] = dirs[i];
			    break;
			}
			if (t == KNIGHT || t == KING) break; // Leapers
			s = Square(s + dirs[i]); // Sliders
		    }
		}
	    }
	}
    }
}

void Game::add_piece(Color c, PieceType t, Square s) {
    int i = pieces.get_nb_pieces(c, t);
    pieces.set_position(c, t, i, s);
    board.set_piece(Piece(c, t, i), s);
    pieces.inc_nb_pieces(c, t);
    
    // Update Zobrist hash
    zobrist.update_piece(current_node().hash(), c, t, s);
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
    zobrist.update_piece(current_node().hash(), c, t, emptied);
}

void Game::new_node() {
    // Remove the previous en passant square from the Zobrist hash
    zobrist.update_en_passant(current_node().hash(), 
			      current_node().get_en_passant());
    
    // Take a "snapshot" of the current position
    tree.push();

    // Update the position for a new move
    current_node().inc_ply();
    current_node().change_side();
    zobrist.change_side(current_node().hash());
}

void Game::del_node() {
    // Take back the previous "snapshot"
    tree.pop();
}
