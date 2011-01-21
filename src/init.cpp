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
#include <list>

#include "game.h"

using namespace std;

#define FEN_DEBUG true

/** Initialise the game according to a FEN record.
  * For example the starting position in chess is :
  * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
  */
void Game::init(string fen) {
    assert(fen.length() > 0);
    string::const_iterator it;

    //cout << "Cleaning the board..." << endl;
    board = Board();
    pieces = Pieces();
    
    if (FEN_DEBUG) cout << "FEN: parsing: " << fen << endl;
    // Parse the FEN for discovering pieces
    Square s = A8;
    for (it = fen.begin(); it != fen.end(); ++it) {
	// Space separator
	if (*it == ' ') {
	    // We have found all the pieces we have to found
	    break;
	}
	// Slash separator
	else if (*it == '/') {
	    // This is the begining of a new rank
	    s = Square(s - 0x18);
	}

	// Empty squares
	else if (int(*it) >= '1' && int(*it) <= '8') {
	    // Find the next square
	    s = Square(s + *it - '1' + 1);
	}

	// Non empty square
	else {
	    // define a new piece
	    Color c;
	    PieceType t;
	    switch (*it) {
		case 'p': c = BLACK, t = PAWN; break;
		case 'n': c = BLACK, t = KNIGHT; break;
		case 'b': c = BLACK, t = BISHOP; break;
		case 'r': c = BLACK, t = ROOK; break;
		case 'q': c = BLACK, t = QUEEN; break;
		case 'k': c = BLACK, t = KING; break;
		case 'P': c = WHITE, t = PAWN; break;
		case 'N': c = WHITE, t = KNIGHT; break;
		case 'B': c = WHITE, t = BISHOP; break;
		case 'R': c = WHITE, t = ROOK; break;
		case 'Q': c = WHITE, t = QUEEN; break;
		case 'K': c = WHITE, t = KING; break;
		default: break;
	    }

	    add_piece(c, t, s);
	    
	    // Update the board's Zobrist key
	    //board.zobrist.set_piece(c, pt, s);

	    // Find the next square
	    s = Square(s + 1);
	}
    }
    ++it;

    // Set the side to move
    switch(*it) {
	case 'w':
	    //current_node().change_side();
	    if (FEN_DEBUG) cout << "FEN: white to play" << endl;
	    break;
	case 'b':
	    current_node().change_side();
	    if (FEN_DEBUG) cout << "FEN: black to play" << endl;
	    break;
	default:
	    assert(!"FEN: could not found the site to move!");
	    break;
    }
    ++it; // Skip the space char
    ++it;

    // Parse the FEN for discovering castling abilities
    for (; it != fen.end(); ++it) {

	//cout << "FEN: parse castle: " << *it << endl;
	if (*it == ' ') {
	    break; // Exit loop
	}
	switch(*it) {
	    case '-': break;
	    case 'K': current_node().set_castle_right(WHITE, KING); break;
	    case 'Q': current_node().set_castle_right(WHITE, QUEEN); break;
	    case 'k': current_node().set_castle_right(BLACK, KING); break;
	    case 'q': current_node().set_castle_right(BLACK, QUEEN); break;
	}
    }
    ++it;

    // Set the en passant square if any
    if (*it != '-') {
	char file = *it;
	++it;
	char rank = *it;
	Square s = Square((rank - '1') * 16 + file - 'a');
	current_node().set_en_passant(s);
	if (FEN_DEBUG) {
	    cout << "FEN: en passant square fixed to ";
	    cout << static_cast<char>(97 + (s & 7));
	    cout << 1 + (s >> 4) << endl;
	}
    }
    ++it; // Skip the space char
    ++it;

    // Parse the FEN for setting the 50 moves counter
    int half_move = 0;
    for (; it != fen.end(); ++it) {
	// Space separator
	if (*it == ' ') {
	    //current_node().set_repetitions(half_move);
	    if (FEN_DEBUG) {
		cout << "FEN: board repetition fixed to ";
		cout << half_move << endl;
	    }
	    break;
	}
	else {
	    half_move = half_move * 10 + *it - '0';
	}
    }
    ++it;	
}
