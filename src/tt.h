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

#ifndef TT_H
#define TT_H

#include <string>

#include "common.h"
#include "hashtable.h"
#include "move.h"
#include "zobrist.h"

class Transposition
{
    private:
        short value;         // 16 bits needed
        Move best_move;      // 16 bits needed
        unsigned char depth; // 10 bits needed (depth < 512)
        Bound bound;         // 3 bits needed

    public:
        Transposition(int v, Bound b, int d, Move bm)
            : value(v), best_move(bm), depth(d),
              bound(b) {}
        Transposition()
            : value(0), best_move(Move()), depth(0),
              bound(UNDEF_BOUND) {}

        int get_value() const { return value; };
        int get_depth() const { return depth; };
        Bound get_bound() const { return bound; };
        Move get_best_move() const { return best_move; };

        bool is_empty() const {
            return value == 0 &&
                   depth == 0 &&
                   bound == UNDEF_BOUND &&
                   best_move.is_null();
        };

        std::string to_string() const;
};

class Transpositions : public HashTable<Transposition>
{
    private:
        const Transposition NULL_ENTRY;

    public:
        Transpositions(int n = TT_SIZE) : HashTable<Transposition>(n) {}

        void save(Hash h, int v, Bound b, int d, Move bm) {
            HashTable<Transposition>::save(h, Transposition(v, b, d, bm));
        };
};

#endif /* !TT_H */
