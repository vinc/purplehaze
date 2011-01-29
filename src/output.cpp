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

void Game::print_thinking(int ply, int score, Move m) {
    cout << setw(4) << ply;
    cout << setw(WIDE) << score;
    cout << setw(WIDE) << int(time.get_elapsed_time() * 100);
    cout << setw(WIDE) << nodes_count;
    cout << setw(WIDE - 3) << " ";
    print_principal_variation(ply, m);
    cout << endl;
}

void Game::print_principal_variation(int depth, Move m) {
    cout << " ";
    int ply = current_node().get_ply();
    if (ply % 2 == 0) cout << 1 + (ply / 2) << ". ";
    print_move(m);
    make_move(m);
    if (is_check(current_node().get_turn_color())) cout << "+";
    
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
    if (depth > 0 && !move.is_null()) {
	print_principal_variation(depth - 1, move);
    }

    undo_move(m);
}

void Game::print_move(Move m) {
    Square s = m.get_orig();
    Piece p = board.get_piece(s);
    PieceType t = p.get_type();
    if (t > PAWN) cout << Piece(WHITE, t).to_string(); // Upper case
    cout << char('a' + m.get_dest_file());
    cout << char('1' + m.get_dest_rank());
}
