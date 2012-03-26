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
        short value;         // 2 bytes / 16 bits used
        Move best_move;      // 2 bytes / 16 bits used
        unsigned char depth; // 1 byte  /  8 bits used: depth < 256
        Bound bound;         // 1 byte  /  2 bits used

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

        bool operator==(const Transposition& other) const {
            return this->value == other.value &&
                   this->depth == other.depth &&
                   this->bound == other.bound &&
                   this->best_move == other.best_move;
        }
        bool operator!=(const Transposition& other) const {
            return !(*this == other);
        }

        std::string to_string() const;
};

class Transpositions : public HashTable<Transposition>
{
    private:
        static const Transposition NULL_ENTRY;

    public:
        Transpositions(int n = TT_SIZE) : HashTable<Transposition>(n) {}

        void save(Hash h, int v, Bound b, int d, Move bm) {
            HashTable<Transposition>::save(h, Transposition(v, b, d, bm));
        };
};

#endif /* !TT_H */
