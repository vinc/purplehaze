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

#ifndef GAME_H
#define GAME_H

#include <string>

#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "tt.h"
#include "hashtable.h"
#include "time.h"
#include "positions.h"
#include "zobrist.h"
#include "search.h"

class Position;

class Game
{
    private:
        Zobrist zobrist;
        Move killer_moves[MAX_PLY][MAX_KILLERS];
        HashTable<Material> material_table;

    public:
        bool output_thinking;
        unsigned int nodes_count; // Used in search
        Pieces pieces;
        Board board;
        Positions positions;
        Time time;
        Transpositions tt;
        MoveList search_moves;

        Game(const int tt_size = TT_SIZE, const int mt_size = MT_SIZE) :
            material_table(mt_size),
            output_thinking(false),
            nodes_count(0),
            tt(tt_size)
            {
                init_eval(); // PST
                Moves::init_mvv_lva_scores();
            }

        void add_piece(Color c, PieceType t, Square s);
        void del_piece(Color c, PieceType t, int i);
        void del_piece(Piece p) {
            del_piece(p.color(), p.type(), p.index());
        };

        void new_position();
        void del_position();

        std::string fen(); // Get FEN record of position
        void fen(std::string record); // Set position from FEN record

        bool is_check(Color c) const {
            Square s = pieces.position(c, KING, 0);
            return board.is_attacked_by(!c, s, pieces);
        }

        void make_move(Move m);
        void undo_move(Move m);
        bool is_legal(Move m);
        bool is_dangerous(Move m);

        // Search
        unsigned long long int perft(unsigned int depth);

        int quiescence(int alpha, int beta, int depth, const int ply);

        template<NodeType node_type>
        int search(int alpha, int beta, int depth, const int ply);

        Move root(int max_depth);

        // Killer Moves
        void clear_killers();
        /*
        Move killer(int depth, int index) {
            return killer_moves[depth][index];
        };
        */
        Move (&killers(const int depth))[MAX_KILLERS] {
            return killer_moves[depth];
        };
        void set_killer(const Move move, const int depth);
        bool is_killer(const Move move, const int depth) {
            return (move == killer_moves[depth][0] ||
                    move == killer_moves[depth][1]);
        };

        // Position's evaluation
        void init_eval();
        int eval(int alpha, int beta);
        int material_eval();
        int position_eval();

        // Output
        void print_thinking_header();
        void print_thinking(int depth, int score, Move m);
        std::string output_pv(int depth, int score, Move m);
        std::string output_move(Move m);
        std::string output_square(Square s) {
            return output_square(board.file(s), board.rank(s));
        };
        std::string output_square(File f, Rank r);
        std::string debug_move(Move m);

        template <class T>
        std::string output_hashtable_stats(const HashTable<T> &table);

        std::string output_mt_stats() {
            return output_hashtable_stats(material_table);
        }
        std::string output_tt_stats() {
            return output_hashtable_stats(tt);
        }
};

#endif /* !GAME_H */
