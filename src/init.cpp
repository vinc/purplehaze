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
#include <sstream>
#include <string>

#include "game.h"

/*
 * Initialise the game according to a FEN record.
 * For example the starting position in chess is:
 * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
 */
void Game::init(std::string fen)
{
    assert(fen.length() > 0);
    std::istringstream iss(fen);

    // Initialize objects
    board = Board();
    pieces = Pieces();
    tree = Tree();

    // Parse board positions
    Square s = A8;
    std::string positions;
    iss >> positions;
    for (auto it = positions.begin(); it != positions.end(); ++it) {
        char sq = *it;
        if (sq == '/') {
            s = Square(s + DOWN + 8 * LEFT); // New rank
        } else if ('1' <= sq && sq <= '8') { // Empty squares
            s = Square(s + sq - '1' + 1); // Next square
        } else { // Non empty square
            Color c = WHITE;
            PieceType t = EMPTY;
            switch (sq) {
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
                default: assert(false);
            }
            add_piece(c, t, s);
            s = Square(s + RIGHT); // Next square
        }
    }
    assert(s == Square(H1 + RIGHT));

    // Set the side to move
    char side;
    iss >> side;
    assert(current_position().side() == WHITE);
    switch(side) {
        case 'w': break;
        case 'b': current_position().change_side(); break;
        default: assert(!"FEN: no side to move!"); break;
    }

    std::string castling;
    iss >> castling;
    for (auto it = castling.begin(); it != castling.end(); ++it) {
        switch(*it) {
            case '-': break;
            case 'K': current_position().set_castle_right(WHITE, KING); break;
            case 'Q': current_position().set_castle_right(WHITE, QUEEN); break;
            case 'k': current_position().set_castle_right(BLACK, KING); break;
            case 'q': current_position().set_castle_right(BLACK, QUEEN); break;
        }
    }

    std::string ep;
    iss >> ep;
    if (ep != "-") {
        char file = ep.at(0);
        char rank = ep.at(1);
        s = Square((rank - '1') * 16 + file - 'a');
        assert(!board.is_out(s));
        current_position().set_en_passant(s);
    }

    int halfmove = 0;
    iss >> halfmove;
    current_position().set_halfmove(halfmove);

    int fullmove = 1;
    iss >> fullmove;
    int ply = 2 * (fullmove - 1);
    if (current_position().side() == BLACK) ++ply;
    tree.set_ply(ply);
}
