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

#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <iostream>
#include <random>

#include "common.h"

using namespace std;

typedef uint64_t Hash;

class Zobrist
{
    friend ostream& operator<<(ostream& out, const Zobrist& zobrist);

    private:
        Hash gen_hash();
        Hash pieces_positions[2][7][BOARD_SIZE];
        Hash side_to_move;
        Hash castle_rights[4];
        Hash en_passants[BOARD_SIZE];
        mt19937_64 generator;
    public:
        Zobrist();
        void change_side(Hash& h) {
            h ^= side_to_move;
        };
        void update_piece(Hash& h, Color c, PieceType t, Square s) {
            h ^= pieces_positions[c][t][s];
        };
        void update_castle_right(Hash& h, Color c, PieceType t) {
            h ^= castle_rights[2 * c + t - QUEEN];
        };
        void update_en_passant(Hash& h, Square s) {
            h ^= en_passants[s];
        };
};

#endif /* !ZOBRIST_H */
