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

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "board.h"

Board::Board() : board(), dir_array()
{

    // Initialize attack and direction arrays
    for (const Square &from : SQUARES) {
        if (from == OUT) {
            break;
        }
        for (const Square &to : SQUARES) {
            if (to == OUT) {
                break;
            }
            int diff = 0x77 + from - to;
            for (const PieceType& t : NOT_PAWN_TYPES) {
                for (const Direction &d : PIECES_DIRS[t]) {
                    if (d == NO_DIR) {
                        break;
                    }
                    Square s = static_cast<Square>(from + d);
                    while (!is_out(s)) {
                        if (s == to) {
                            attack_array[diff][t] = true;
                            dir_array[diff] = d;
                            break;
                        }
                        switch (t) {
                        case KNIGHT:
                        case KING:
                            s = OUT;
                            break;
                        default:
                            s = static_cast<Square>(s + d);
                            break;
                        }
                    }
                }
            }
        }
    }
}

/*
 * Generate a string for pretty printing the content of a 0x88 string array.
 * This could be the board but also any PST array.
 */
std::string Board::to_string(const std::string squares[], const int sq_width)
{
    std::ostringstream stream;
    stream << std::endl;
    for (Square s = A8; s < OUT; s = Square(s + 1)) {
        if (is_out(s)) {
            continue;
        }
        if (file(s) == FILE_A) {
            stream << "     +";
            for (int i = 0; i < 8; ++i) {
                // Every string representing a square
                // should have the same size.
                assert(sq_width > 0);
                assert(squares[s].size() ==
                       static_cast<unsigned int>(sq_width));
                for (int j = 0; j < sq_width; ++j) {
                    stream << "-";
                }
                stream << "+";
            }
            stream << std::endl;
            stream << "   " << rank(s) + 1 << " ";
        }
        stream << "|";
        stream << squares[s];
        if (file(s) == FILE_H) {
            stream << "|" << std::endl;
            if (s == H1) {
                break; // The loop ends here
            }
            s = Square(s - 0x18);
        }
    }

    // Bottom's border of the array
    stream << "     +";
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < sq_width; ++j) {
            stream << "-";
        }
        stream << "+";
    }
    stream << std::endl << "     ";

    // Output files names
    for (char c = 'a'; c <= 'h'; ++c) {
        int l = sq_width / 2;
        int r = sq_width % 2;
        stream << std::setw(l + 2) << c << std::setw(r) << " ";
    }
    stream << std::endl;
    return stream.str();
}

/*
 * Pretty print the board
 */
std::ostream& operator<<(std::ostream& out, const Board board)
{
    std::string squares[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i) {
        Square s = Square(i);
        squares[i] = " ";
        if (!board.is_empty(s)) {
            squares[i] += board[s].to_string();
        } else if (board.is_dark(s)) {
            squares[i] += ".";
        } else {
            squares[i] += " ";
        }
        squares[i] += " ";
    }
    out << board.to_string(squares, 3);
    return out;
}
