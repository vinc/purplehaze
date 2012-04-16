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

#ifndef MOVE_H
#define MOVE_H

#include <string>

#include "common.h"

class Move
{
    protected:
        /*
         * A move is coded using 16 bits:
         *     4 bits for the type
         *     3 bits for the destination square rank
         *     3 bits for the destination square file
         *     3 bits for the origin square rank
         *     3 bits for the origin square file
         */

        static const int MT_MASK = 0xF;
        static const int DR_MASK = 0x7;
        static const int DF_MASK = 0x7;
        static const int OR_MASK = 0x7;
        static const int OF_MASK = 0x7;

        static const int MT_SHIFT = 0;
        static const int DR_SHIFT = 4;
        static const int DF_SHIFT = 7;
        static const int OR_SHIFT = 10;
        static const int OF_SHIFT = 13;

        uint16_t code;

        bool is_set(int i) const {
            return code & ( 1 << i);
        };

    public:
        Move() : code(NULL_MOVE) {}

        Move(Square o, Square d, MoveType t) {
            code = ((o & 7) << OF_SHIFT) |
                   ((o >> 4) << OR_SHIFT) |
                   ((d & 7) << DF_SHIFT) |
                   ((d >> 4) << DR_SHIFT) |
                   t;
        };

        File orig_file() const {
            return File((code >> OF_SHIFT) & OF_MASK);
        };
        Rank orig_rank() const {
            return Rank((code >> OR_SHIFT) & OR_MASK);
        };
        File dest_file() const {
            return File((code >> DF_SHIFT) & DF_MASK);
        };
        Rank dest_rank() const {
            return Rank((code >> DR_SHIFT) & DR_MASK);
        };
        Square orig() const {
            return Square(16 * orig_rank() + orig_file());
        };
        Square dest() const {
            return Square(16 * dest_rank() + dest_file());
        };
        MoveType type() const {
            return static_cast<MoveType>((code >> MT_SHIFT) & MT_MASK);
        };

        bool is_capture() const {
            return is_set(2) && !is_null();
        };
        bool is_castle() const {
            return !is_set(3) && !is_set(2) && is_set(1);
        };
        bool is_promotion() const {
            return is_set(3);
        };
        bool is_double_pawn_push() const {
            return type() == DOUBLE_PAWN_PUSH;
        };
        bool is_en_passant() const {
            return type() == EN_PASSANT;
        };
        bool is_null() const {
            return type() == NULL_MOVE;
        };
        PieceType promotion_type() const;
        PieceType castle_side() const;

        /*
        // Static member function for sorting move in natural order
        static bool numeric_comp(Move a, Move b) {
            return (a.code < b.code);
        };
        */

        bool operator==(const Move& other) const {
            return this->code == other.code;
        }
        bool operator!=(const Move& other) const {
            return !(*this == other);
        }
        std::string to_string() const;

        friend std::ostream& operator<<(std::ostream& out, const Move move);
        friend class ExtendedMove;
};

class ExtendedMove : public Move
{
    private:
        char val;

    public:
        ExtendedMove() : val(0) { code = NULL_MOVE; }
        ExtendedMove(Move m, int v = 0) : val(v) { code = m.code; }

        char value() const {
            return val;
        };
        bool operator<(const ExtendedMove& other) const {
            return this->val > other.val;
        }
};

#endif /* !MOVE_H */
