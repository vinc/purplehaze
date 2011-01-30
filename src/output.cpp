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
#include <iomanip>
#include <string>
#include <sstream>

#include "game.h"

using namespace std;

#define WIDE 10

void Game::print_thinking_header() {
    cout << setw(4) << "ply";
    cout << setw(WIDE) << "score";
    cout << setw(WIDE) << "time";
    cout << setw(WIDE) << "nodes";
    cout << setw(WIDE) << "pv";
    cout << endl;
}

void Game::print_thinking(int depth, int score, Move m) {
    cout << setw(4) << depth;
    cout << setw(WIDE) << score;
    cout << setw(WIDE) << int(time.get_elapsed_time() * 100);
    cout << setw(WIDE) << nodes_count;
    cout << setw(WIDE - 3) << " ";
    int ply = current_node().get_ply();
    if (ply % 2 != 0) cout << 1 + (ply / 2) << ". ...";
    cout << output_principal_variation(depth, m) << endl;
}

string Game::output_principal_variation(int depth, Move m) {
    ostringstream stream;
    stream << " ";
    int ply = current_node().get_ply();
    if (ply % 2 == 0) stream << 1 + (ply / 2) << ". ";
    stream << output_move(m);
    make_move(m);
    if (is_check(current_node().get_turn_color())) stream << "+";
    
    // Find next move in TT
    Transposition trans = tt.lookup(current_node().hash());
    /*
    cout << endl;
    cout << "value=" << trans.get_value();
    cout << ", bound=" << trans.get_bound();
    cout << ", depth=" << trans.get_depth();
    cout << ", move=" << trans.get_best_move() << endl;
    */
    Move move = trans.get_best_move();
    if (depth > 0 && is_legal(move) && trans.get_bound() < 3) {
	stream << output_principal_variation(depth - 1, move);
    }

    undo_move(m);
    return stream.str();
}

string Game::output_move(Move m) {
    ostringstream stream;

    // Castling
    if (m.is_castle()) {
	if (m.get_castle_side() == QUEEN) stream << "O-";
	return stream.str() + "O-O";
    }

    // Type of piece
    Square from = m.get_orig();
    Piece p = board.get_piece(from);
    PieceType t = p.get_type();
    if (t > PAWN) stream << Piece(WHITE, t); // Upper case
    
    // Capture
    if (m.is_capture()) {
	if (t == PAWN) stream << char('a' + m.get_orig_file());
	else { // Disambiguation
	    Color c = current_node().get_turn_color();
	    Square to = m.get_dest();
	    for (int i = 0; i < pieces.get_nb_pieces(c, t); ++i) {
		Square s = pieces.get_position(c, t, i);
		if (s != from && can_attack(t, s, to)) {
		    stream << char('a' + m.get_orig_file());
		    break;
		}
	    }
	}
	stream << "x";
    }
    
    // Destination
    stream << char('a' + m.get_dest_file()) << char('1' + m.get_dest_rank());
    
    // Promotion
    if (m.is_promotion()) {
	stream << "=" << Piece(WHITE, m.get_promotion_type());
    }

    return stream.str();
}
