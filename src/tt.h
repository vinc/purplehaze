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

#ifndef TT_H
#define TT_H

#include <string>

#include "common.h"
#include "hashtable.h"
#include "move.h"

class Transposition
{
    private:
        int16_t value_;       // 2 bytes / 16 bits used
        Move best_move_;      // 2 bytes / 16 bits used
        unsigned char depth_; // 1 byte  /  8 bits used (depth < 256)
        Bound bound_;         // 1 byte  /  2 bits used

    public:
        Transposition(int v, Bound b, int d, Move bm)
            : value_(v), best_move_(bm), depth_(d),
              bound_(b) {}
        Transposition()
            : value_(0), best_move_(Move()), depth_(0),
              bound_(UNDEF_BOUND) {}

        int value() const {
            return value_;
        };
        int depth() const {
            return depth_;
        };
        Bound bound() const {
            return bound_;
        };
        Move best_move() const {
            return best_move_;
        };

        bool is_empty() const {
            return value_ == 0 &&
                   depth_ == 0 &&
                   bound_ == UNDEF_BOUND &&
                   best_move_.is_null();
        };

        bool operator==(const Transposition& other) const {
            return this->value_ == other.value_ &&
                   this->depth_ == other.depth_ &&
                   this->bound_ == other.bound_ &&
                   this->best_move_ == other.best_move_;
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
