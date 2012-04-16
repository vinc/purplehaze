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

#ifndef MOVES_H
#define MOVES_H

#include "common.h"
#include "move.h"

class Board;
class Pieces;
class Position;

typedef char Score;

const Score BEST_SCORE = 127;
const Score KILLERS_SCORE = 0;

const int MOVES_STATE_SIZE = 5;
enum MovesState : unsigned char {
    BEST, GOOD_CAPTURES, KILLERS, BAD_CAPTURES, QUIET_MOVES, UNDEF_MOVES
};

class MoveList
{
    private:
        ExtendedMove list[MAX_PLY][MAX_BF];
        unsigned int ply;

    public:
        MoveList() :
            ply(0)
            {
                for (int i = 0; i < MAX_PLY; ++i) {
                    for (int j = 0; j < MAX_BF; ++j) {
                        list[i][j] = ExtendedMove();
                    }
                }
            }
        void inc_ply() {
            ++ply;
        };
        void dec_ply() {
            --ply;
        };
        void clear() {
            ply = 0;
        };
        ExtendedMove& operator[] (unsigned char i) {
            return list[ply][i];
        };
};

class Moves
{
    private:
        static Score mvv_lva_scores[NB_PIECE_TYPES][NB_PIECE_TYPES];

        MoveList& moves;
        const Position& current_position;
        const Board& board;
        const Pieces& pieces;

        unsigned char size[MOVES_STATE_SIZE]; // Moves types counters
        unsigned char i, n;
        MovesState generation_state;
        bool use_lazy_generation;

    public:
        Moves(const Board& b, const Pieces& ps, const Position& cn,
              MoveList& ml, bool lg = true) :
            moves(ml), current_position(cn), board(b), pieces(ps),
            i(0), n(0),
            generation_state(BEST),
            use_lazy_generation(lg)
            {
                moves.inc_ply(); // Increment move list internal counter
                for (int j = 0; j < MOVES_STATE_SIZE; ++j) size[j] = 0;
            }

        ~Moves() {
            moves.dec_ply(); // Decrement move list internal counter
        }

        void generate(MoveType mt = NULL_MOVE); // here NULL_MOVE => ALL_MOVE
        void generate_pieces(Color c, PieceType t, MoveType mt);
        void add(Move m, MovesState mt = UNDEF_MOVES);
        ExtendedMove next();
        MovesState state() const {
            return generation_state;
        };

        static void init_mvv_lva_scores();
        Score get_mvv_lva_score(Move m);

        /*
        // Used in divide
        void numeric_sort();
        */
};

#endif /* !MOVES_H */
