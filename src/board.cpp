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

#include <iostream>
#include <sstream>
#include <iomanip>

#include "board.h"

using namespace std;

Board::Board() {
    // Initialize board
    for (int i = 0; i < BOARD_SIZE; ++i) board[i] = Piece();   

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
		for (int d = 0; d < NB_DIRS[t]; ++d) {
		    Square s = Square(from + dirs[d]);
		    while (!is_out(s)) {
			if (s == to) {
			    attack_array[diff].set(t, true);
			    dir_array[diff] = dirs[d];
			    break;
			}
			if (t == KNIGHT || t == KING) break; // Leapers
			s = Square(s + dirs[d]); // Sliders
		    }
		}
	    }
	}
    }
}

string Board::to_string(string squares[]) const {
    ostringstream stream;
    stream << endl;
    for (Square s = A8; ; s = Square(s + 1)) {
	if (is_out(s)) continue;
	if (get_file(s) == FILE_A) {
	    stream << "     +";
	    for (int i = 0; i < 8; ++i) {
		for (unsigned int j = 0; j < squares[s].size(); ++j) {
		    stream << "-";
		}
		stream << "+";
	    }
	    stream << endl;
	    stream << "   " << get_rank(s) + 1 << " ";
	}
	stream << "|";
	stream << squares[s];
	if (get_file(s) == FILE_H) {
	    stream << "|" << endl;
	    if (s == H1) break;
	    else s = Square(s - 0x18);
	}
    }
    stream << "     +";
    for (int i = 0; i < 8; ++i) {
	for (unsigned int j = 0; j < squares[A1].size(); ++j) {
	    stream << "-";
	}
	stream << "+";
    }
    stream << endl << "     ";
    for (char c = 'a'; c <= 'h'; ++c) {
	int l = squares[A1].size() / 2;
	int r = squares[A1].size() % 2;
	stream << setw(l + 2) << c << setw(r) << " ";
    }
    stream << endl;
    //stream << "     +---+---+---+---+---+---+---+---+" << endl;
    //stream << "       a   b   c   d   e   f   g   h  " << endl;
    return stream.str();
}

ostream& operator<<(ostream& out, const Board board) {
    /*
    for (Square s = A8; ; s = Square(s + 1)) {
	if (board.is_out(s)) continue;
	if (board.get_file(s) == FILE_A) {
	    out << "     +---+---+---+---+---+---+---+---+" << endl;
	    out << "   " << board.get_rank(s) + 1 << " ";
	}
	out << "| ";
	if (!board.is_empty(s)) out << board.get_piece(s);
	else if (board.is_dark(s)) out << ".";
	else out << " ";
	out << " ";
	if (board.get_file(s) == FILE_H) {
	    out << "|" << endl;
	    if (s == H1) break;
	    else s = Square(s - 0x18);
	}
    }
    out << "     +---+---+---+---+---+---+---+---+" << endl;
    out << "       a   b   c   d   e   f   g   h  " << endl;
    return out;
    */

    string squares[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i) {
	Square s = Square(i);
	squares[i] = " ";
	if (!board.is_empty(s)) squares[i] += board.get_piece(s).to_string();
	else if (board.is_dark(s)) squares[i] += ".";
	else squares[i] += " ";
	squares[i] += " ";
    }
    out << board.to_string(squares);
    return out;
}
