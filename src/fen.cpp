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
#include <sstream>
#include <string>

#include "game.h"

/*
 * Initialise the game according to a FEN record.
 * For example the starting position in chess is:
 * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
 */
void Game::fen(std::string record)
{
    assert(record.length() > 0);
    std::istringstream iss(record);

    // Initialize objects
    board = Board();
    pieces = Pieces();
    positions = Positions();

    // Parse board positions
    Square s = A8;
    std::string placement;
    iss >> placement;
    for (auto it = placement.begin(); it != placement.end(); ++it) {
        char sq = *it;
        if (sq == '/') {
            s = Square(s + DOWN + 8 * LEFT); // New rank
        } else if ('1' <= sq && sq <= '8') { // Empty squares
            s = Square(s + sq - '1' + 1); // Next square
        } else { // Non empty square
            Color c = WHITE;
            PieceType t = EMPTY;
            switch (sq) {
            case 'p':
                c = BLACK;
                t = PAWN;
                break;
            case 'n':
                c = BLACK;
                t = KNIGHT;
                break;
            case 'b':
                c = BLACK;
                t = BISHOP;
                break;
            case 'r':
                c = BLACK;
                t = ROOK;
                break;
            case 'q':
                c = BLACK;
                t = QUEEN;
                break;
            case 'k':
                c = BLACK;
                t = KING;
                break;
            case 'P':
                c = WHITE;
                t = PAWN;
                break;
            case 'N':
                c = WHITE;
                t = KNIGHT;
                break;
            case 'B':
                c = WHITE;
                t = BISHOP;
                break;
            case 'R':
                c = WHITE;
                t = ROOK;
                break;
            case 'Q':
                c = WHITE;
                t = QUEEN;
                break;
            case 'K':
                c = WHITE;
                t = KING;
                break;
            default:
                assert(false);
            }
            add_piece(c, t, s);
            s = Square(s + RIGHT); // Next square
        }
    }
    assert(s == Square(H1 + RIGHT));

    // Set the side to move
    char side;
    iss >> side;
    assert(positions.current().side() == WHITE);
    switch(side) {
    case 'w':
        break;
    case 'b':
        positions.current().change_side();
        break;
    default:
        assert(false);
        break;
    }

    std::string castling;
    iss >> castling;
    for (auto it = castling.begin(); it != castling.end(); ++it) {
        switch(*it) {
        case 'K':
            positions.current().set_castle_right(WHITE, KING);
            break;
        case 'Q':
            positions.current().set_castle_right(WHITE, QUEEN);
            break;
        case 'k':
            positions.current().set_castle_right(BLACK, KING);
            break;
        case 'q':
            positions.current().set_castle_right(BLACK, QUEEN);
            break;
        case '-':
            break;
        }
    }

    std::string ep;
    iss >> ep;
    if (ep != "-") {
        char file = ep.at(0);
        char rank = ep.at(1);
        s = Square((rank - '1') * 16 + file - 'a');
        assert(!board.is_out(s));
        positions.current().set_en_passant(s);
    }

    int halfmove = 0;
    iss >> halfmove;
    positions.current().set_halfmove(halfmove);

    int fullmove = 1;
    iss >> fullmove;
    int ply = 2 * (fullmove - 1);
    if (positions.current().side() == BLACK) {
        ++ply;
    }
    positions.set_ply(ply);
}

std::string Game::fen()
{
    std::ostringstream record;
    const Position& pos = positions.current();

    for (int i = 0; i < 8; ++i) {
        if (i) {
            record << '/';
        }
        int n = 0;
        for (int j = 0; j < 8; ++j) {
            const Square s = SQUARES[((7 - i) * 8) + j];
            if (board.is_empty(s)) {
                ++n;
            } else {
                if (n) {
                    record << n;
                    n = 0;
                }
                record << board[s].to_string();
            }
        }
        if (n) {
            record << n;
        }
    }

    record << ' ';

    switch (pos.side()) {
    case WHITE:
        record << 'w';
        break;
    case BLACK:
        record << 'b';
        break;
    }

    record << ' ';

    const PieceType sides[] = { KING, QUEEN };
    bool found_castle = false;
    for (const Color& c : COLORS) {
        for (const PieceType& t : sides) {
            if (pos.can_castle(c, t)) {
                record << Piece(c, t).to_string();
                found_castle = true;
            }
        }
    }
    if (!found_castle) {
        record << '-';
    }

    record << ' ';

    const Square ep = pos.en_passant();
    switch (ep) {
    case OUT:
        record << '-';
        break;
    default:
        record << output_square(Board::file(ep), Board::rank(ep));
        break;
    }

    record << ' ' << static_cast<int>(pos.halfmove())
           << ' ' << (1 + positions.ply() / 2);
    return record.str();
}
