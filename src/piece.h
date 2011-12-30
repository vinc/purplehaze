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

#ifndef PIECE_H
#define PIECE_H

#include <bitset>
#include <string>

#include "common.h"

using namespace std;

class Piece
{
    friend ostream& operator<<(ostream& out, const Piece piece);
    private:
        /*
         * A piece is coded using 8 bits:
         *     1 bit for the color
         *     3 bits for the type
         *     4 bits for the index in pieces[c][t][i]
         *
         * FIXME? No warning given if index is > 4 bits
         */
        static const int C_MASK = 0x1;
        static const int T_MASK = 0x7;
        static const int I_MASK = 0xF;
        static const int C_SHIFT = 0;
        static const int T_SHIFT = 1;
        static const int I_SHIFT = 4;

        unsigned char code;

    public:
        Piece() : code(EMPTY) {}

        Piece(Color c, PieceType t, int i = 0) {
            code = ((i /*& I_MASK*/) << I_SHIFT) |
                   ((static_cast<int>(t) /*& T_MASK*/) << T_SHIFT) |
                   ((static_cast<int>(c) /*& C_MASK*/) << C_SHIFT);
        }

        Color get_color() const {
            return static_cast<Color>((code >> C_SHIFT) & C_MASK);
        };
        PieceType get_type() const {
            return static_cast<PieceType>((code >> T_SHIFT) & T_MASK);
        };
        int get_index() const {
            return (code >> I_SHIFT) & I_MASK;
        };
        void set_index(int i) {
            code &= I_MASK;
            code |= (i << I_SHIFT);
        }
        bool operator==(const Piece& other) const {
            return this->code == other.code;
        }
        bool operator!=(const Piece& other) const {
            return !(*this == other);
        }
        string to_string() const;
};

#endif /* !PIECE_H */
