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
//#include <iostream>

#include "game.h"

//using namespace std;

Game::Game() {
    tree.push(Node());
}

void Game::add_piece(Color c, PieceType t, Square s) {
    int i = pieces.get_nb_pieces(c, t);
    pieces.set_position(c, t, i, s);
    board.set_piece(Piece(c, t, i), s);
    pieces.inc_nb_pieces(c, t);
}

void Game::del_piece(Color c, PieceType t, int i) {
    // Remove the piece, and put in its place the higher index piece of the
    // same color and type in order to avoid holes (idea from Mediocre Chess)
    Square emptied = pieces.get_position(c, t, i);
    board.set_piece(Piece(), emptied);
    pieces.dec_nb_pieces(c, t);
    if (pieces.get_nb_pieces(c, t) > 0) {
	Square s = pieces.get_position(c, t, pieces.get_nb_pieces(c, t));
	pieces.set_position(c, t, i, s);
	board.set_piece(Piece(c, t, i), s);
    }
}

void Game::new_node() {
    tree.push(tree.top());
    tree.top().change_side();
    tree.top().inc_ply();
}

void Game::del_node() {
    //cout << "tree.pop()" << endl;
    tree.pop();
}
