/* Copyright (C) 2007-2012 Vincent Ollivier
 *
 * Purple Haze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Purple Haze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <iostream>

#include "game.h"

void Game::clear_killers()
{
    for (int i = 0; i < MAX_PLY; ++i) {
        for (int j = 0; j < MAX_KILLERS; ++j) {
            killer_moves[i][j] = Move();
        }
    }
}

void Game::add_piece(Color c, PieceType t, Square s)
{
    const int i = pieces.count(c, t);
    pieces.set_position(c, t, i, s);
    board[s] = Piece(c, t, i);
    pieces.inc_nb_pieces(c, t);

    // Update Zobrist hash
    Position& pos = current_position();
    zobrist.update_piece(pos.hash(), c, t, s);

    // Hack: for the material hash, the position is irrelevant but each piece
    // needs a unique hash so we are using Square(i) in place of the position.
    // Remove the previous total
    zobrist.update_piece(pos.material_hash(), c, t, Square(i));
    // Add the new total
    zobrist.update_piece(pos.material_hash(), c, t, Square(i + 1));
}

void Game::del_piece(Color c, PieceType t, int i)
{
    // Remove the piece, and put in its place the higher index piece of the
    // same color and type in order to avoid holes (idea from Mediocre Chess)
    const Square emptied = pieces.position(c, t, i); // Get piece's position
    board[emptied] = Piece();                        // Remove it from board
    pieces.dec_nb_pieces(c, t);                      // and from pieces list
    const int j = pieces.count(c, t);                // Last piece's index
    if (i != j && pieces.count(c, t) > 0) {
        // Swap i and j and update board
        Square s = pieces.position(c, t, j);         // Last piece's position
        pieces.set_position(c, t, i, s);             // Fill the hole left
        board[s] = Piece(c, t, i);                   // Update board
    }
    // Update Zobrist hash
    Position& pos = current_position();
    zobrist.update_piece(pos.hash(), c, t, emptied);

    // Same hack here for the material hash than in add_piece()
    zobrist.update_piece(pos.material_hash(), c, t, Square(j + 1));
    zobrist.update_piece(pos.material_hash(), c, t, Square(j));
}

void Game::new_position()
{
    // Save the state of the current position
    positions.clone_current();

    // Remove the previous en passant square from the Zobrist hash
    Position& pos = current_position();
    zobrist.update_en_passant(pos.hash(), pos.en_passant());

    // Prepare the position for a new move
    pos.change_side();
    zobrist.change_side(pos.hash());
}

void Game::del_position()
{
    // Revert back to previous position
    positions.delete_current();
}

/*
 * Killer move setter for the Killer Heuristic in move ordering.
 */
void Game::set_killer(const Move move, const int depth)
{
    if (move != killer_moves[depth][0]) {
        killer_moves[depth][1] = killer_moves[depth][0];
        killer_moves[depth][0] = move;
    }
}
