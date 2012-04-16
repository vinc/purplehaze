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

        Piece& operator[] (Square s) {
            return board[s];
        };
        const Piece& operator[] (Square s) const {
            return board[s];
        };

        bool is_empty(Square s) const {
            return board[s].type() == EMPTY;
        };
        static bool is_out(Square s) {
            return s & 0x88;
        };
        static bool is_dark(Square s) {
            return (s & 7) % 2 == (s >> 4) % 2;
        };
        static bool is_same_color(Square a, Square b) {
            return (is_dark(a) && is_dark(b)) || (!is_dark(a) && !is_dark(b));
        }
        static File get_file(Square s) {
            return File(s & 7);
        };
        static Rank get_rank(Square s) {
            return Rank(s >> 4);
        };
        static bool is_pawn_begin(Color c, Square s) {
            return (get_rank(s) - RANK_6 * c) == RANK_2;
        };
        static bool is_pawn_end(Color c, Square s) {
            return (get_rank(s) + RANK_8 * c) == RANK_8;
        };
        static bool is_border(Square s) {
            File f = get_file(s);
            Rank r = get_rank(s);
            if (f == FILE_A || f == FILE_H) return true;
            if (r == RANK_1 || r == RANK_8) return true;
            return false;
        };
        static Square get_square(int i) {
            return Square(i + (i & ~7));
        };
        static Square get_square(File f, Rank r) {
            return Square(16 * r + f);
        };
        static Square get_square(Square s, Direction d) {
            return Square(s + d);
        };

        // Used for PST with black's point of view
        static Square flip(Square s) {
            return get_square(get_file(s), Rank(RANK_8 - get_rank(s)));
        }

        // Theoretical answer by array lookup
        bool can_attack(PieceType t, Square from, Square to) const {
            return static_cast<bool>(attack_array[0x77 + from - to][t]);
        }
        Direction get_direction_to(Square from, Square to) const {
            return dir_array[0x77 + from - to];
        }

        // Practical answer
        bool is_attacked_by(Color c, Square s, const Pieces& pieces) const;
        bool can_go(Piece p, Square from, Square to) const;

        friend std::ostream& operator<<(std::ostream& out, const Board board);
};

#endif /* !BOARD_H */
