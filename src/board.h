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

#ifndef BOARD_H
#define BOARD_H

#include <bitset>
#include <string>

#include "common.h"
#include "pieces.h"

class Board
{
    private:
        Piece board[BOARD_SIZE];
        std::bitset<7> attack_array[240];
        Direction dir_array[240];

    public:
        Board();
        static std::string to_string(const std::string squares[],
                                     const int sq_width);

        Piece& operator[] (const Square s) {
            return board[s];
        };
        const Piece& operator[] (const Square s) const {
            return board[s];
        };

        bool is_empty(const Square s) const {
            return board[s].type() == EMPTY;
        };
        static bool is_out(const Square s) {
            return s & 0x88;
        };
        static bool is_dark(const Square s) {
            return (s & 7) % 2 == (s >> 4) % 2;
        };
        static bool is_same_color(const Square a, const Square b) {
            return (is_dark(a) && is_dark(b)) || (!is_dark(a) && !is_dark(b));
        }
        static File file(const Square s) {
            return File(s & 7);
        };
        static Rank rank(const Square s) {
            return Rank(s >> 4);
        };
        static bool is_pawn_begin(const Color c, const Square s) {
            return (rank(s) - RANK_6 * c) == RANK_2;
        };
        static bool is_pawn_end(const Color c, const Square s) {
            return (rank(s) + RANK_8 * c) == RANK_8;
        };
        static bool is_border(const Square s) {
            const File f = file(s);
            const Rank r = rank(s);
            return f == FILE_A || f == FILE_H || r == RANK_1 || r == RANK_8;
        };
        static Square square(const int i) {
            return static_cast<Square>(i + (i & ~7));
        };
        static Square square(const File f, const Rank r) {
            return static_cast<Square>(16 * r + f);
        };
        static Square square(const Square s, const  Direction d) {
            return static_cast<Square>(s + d);
        };

        static Square flip(const Square s) {
            return square(file(s), Rank(RANK_8 - rank(s)));
        }
        static Square flip(const Square s, const Color c) {
            return static_cast<Square>(s + A8 * c);
        }

        // Theoretical answer by array lookup
        bool can_attack(const PieceType t,
                        const Square from, const Square to) const {
            return static_cast<bool>(attack_array[0x77 + from - to][t]);
        }
        Direction direction_to(const Square from, const Square to) const {
            return dir_array[0x77 + from - to];
        }

        // Practical answer (implemented in 'attack.cpp')
        bool is_attacked_by(const Color c, const Square s,
                            const Pieces& pieces) const;
        bool can_go(const Piece p, const Square from, const Square to) const;

        friend std::ostream& operator<<(std::ostream& out, const Board board);
};

#endif /* !BOARD_H */
