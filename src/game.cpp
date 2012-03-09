/* Copyright (C) 2007-2011 Vincent Ollivier
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

#include <assert.h>
#include <iostream>

#include "game.h"

Game::Game() {
    nodes_count = 0;
    output_thinking = false;
    tt.clear();
    material_table.clear();
    clear_killers();
    search_moves.clear();

    // Make PST
    init_eval();

    // Initialize MVV/LVA score array
    Moves::init_mvv_lva_scores();
}

void Game::clear_killers() {
    for (int i = 0; i < MAX_PLY; ++i) {
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
    Position& pos = current_position();
    zobrist.update_piece(pos.hash(), c, t, s);

    // Hack: for the material hash, the position is irrelevant but each piece
    // needs a unique hash so we are using Square(i) in place of the position.
    // Remove the previous total
    zobrist.update_piece(pos.material_hash(), c, t, Square(i));
    // Add the new total
    zobrist.update_piece(pos.material_hash(), c, t, Square(i + 1));
}

void Game::del_piece(Color c, PieceType t, int i) {
    // Remove the piece, and put in its place the higher index piece of the
    // same color and type in order to avoid holes (idea from Mediocre Chess)
    Square emptied = pieces.get_position(c, t, i); // Get piece's position
    board.set_piece(Piece(), emptied);             // Remove it from board
    pieces.dec_nb_pieces(c, t);                    // and from pieces list
    pieces.set_position(c, t, i, OUT);             // TODO: not needed
    int j = pieces.get_nb_pieces(c, t);            // Last piece's index
    if (pieces.get_nb_pieces(c, t) > 0 && i != j) {
        // Swap i and j and update board
        Square s = pieces.get_position(c, t, j);   // Last piece's position
        pieces.set_position(c, t, i, s);           // Fill the hole left
        pieces.set_position(c, t, j, OUT);         // TODO: not needed
        board.set_piece(Piece(c, t, i), s);        // Update board
    }
    // Update Zobrist hash
    Position& pos = current_position();
    zobrist.update_piece(pos.hash(), c, t, emptied);

    // Same hack here for the material hash than in add_piece()
    zobrist.update_piece(pos.material_hash(), c, t, Square(j + 1));
    zobrist.update_piece(pos.material_hash(), c, t, Square(j));
}

void Game::new_position() {
    // Take a "snapshot" of the current position
    tree.push();

    // Remove the previous en passant square from the Zobrist hash
    zobrist.update_en_passant(current_position().hash(),
                              current_position().get_en_passant());

    // Update the position for a new move
    current_position().inc_ply();
    current_position().change_side();
    zobrist.change_side(current_position().hash());
}

void Game::del_position() {
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
