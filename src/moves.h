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
    ExtendedMove list[MAX_PLY][MAX_MOVES];
    unsigned int ply; // TODO: Redundant with Tree::ply()?

public:
    MoveList() : list(), ply(0) {}

    void inc_ply() {
        ++ply;
    }
    void dec_ply() {
        --ply;
    }
    void clear() {
        ply = 0;
    }
    ExtendedMove& operator[] (unsigned char i) {
        return list[ply][i];
    }

    // Only used in unit tests
    int cur_ply() const {
        return ply;
    }
};

class Moves
{
private:
    static Score mvv_lva_scores[NB_PIECE_TYPES][NB_PIECE_TYPES];

    MoveList& moves;
    const Position& current_position;
    const Board& board;
    const Pieces& pieces;
    int cur;
    int end;
    unsigned char size[MOVES_STATE_SIZE]; // Moves types counters
    MovesState generation_state;
    bool use_lazy_generation;

public:
    Moves(const Board& b, const Pieces& ps, const Position& cn,
          MoveList& ml, bool lg = true) :
        moves(ml), current_position(cn), board(b), pieces(ps),
        cur(0), end(0),
        size(),
        generation_state(BEST),
        use_lazy_generation(lg) {
        moves.inc_ply(); // Increment move list internal counter
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
    }

    static void init_mvv_lva_scores();
    Score mvv_lva_score(Move m);

    // Only used in unit tests
    int count(MovesState mt) const {
        return size[mt];
    }

    /*
    // Used in divide
    void numeric_sort();
    */
};

#endif /* !MOVES_H */
