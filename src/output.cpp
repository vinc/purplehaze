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

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <bitset>

#include "game.h"

static const int WIDE = 10;

void Game::print_thinking_header()
{
    if (!output_thinking) return;
    std::cout << std::setw(4) << "ply"
              << std::setw(WIDE - 1) << "score"
              << std::setw(WIDE) << "time"
              << std::setw(WIDE + 3) << "nodes"
              << std::setw(WIDE) << "pv"
              << std::endl;
}

void Game::print_thinking(int depth, int score, Move m)
{
    if (!output_thinking) return;
    std::cout << std::setw(4) << depth
              << std::setw(WIDE - 1) << score
              << std::setw(WIDE) << time.get_elapsed_time()
              << std::setw(WIDE + 3) << nodes_count
              << std::setw(WIDE - 3) << " ";
    const int ply = tree.get_ply();

    if (current_position().turn_color() == BLACK) {
        std::cout << " " << 1 + (ply / 2) << ". ...";
    }

    assert(is_legal(m) || assert_msg(debug_move(m)));
    std::cout << output_pv(depth, score, m) << std::endl;
}

bool is_mate(int score)
{
    return ((score < -INF + MAX_PLY) || (INF - MAX_PLY < score));
}

std::string Game::output_pv(int depth, int score, Move m)
{
    std::ostringstream stream;
    stream << " ";
    const int ply = tree.get_ply();
    if (current_position().turn_color() == WHITE) {
        stream << 1 + (ply / 2) << ". ";
    }
    stream << output_move(m);

    make_move(m);

    bool is_in_check = is_check(current_position().turn_color());

    // Find next move in TT
    bool is_empty;
    Transposition trans = tt.lookup(current_position().hash(), &is_empty);
    Move move = trans.best_move();
    if (depth > 0 && is_legal(move) && trans.bound() < 3) {
        if (is_in_check) stream << "+"; // Check
        stream << output_pv(depth - 1, trans.value(), move);
    } else if (move.is_null() && is_mate(score)) {
        if (is_in_check) stream << "#"; // Mate
    } else if (is_in_check) {
        stream << "+"; // Cut-off
    }

    undo_move(m);
    return stream.str();
}

std::string Game::output_move(Move m)
{
    std::ostringstream stream;

    // Castling
    if (m.is_castle()) {
        if (m.castle_side() == QUEEN) stream << "O-";
        return stream.str() + "O-O";
    }

    // Type of piece
    Square from = m.orig();
    Piece p = board[from];
    PieceType t = p.type();
    if (t > PAWN) stream << Piece(WHITE, t); // Upper case

    // Disambiguation
    if (t != PAWN) {
        Color c = p.color();
        Square to = m.dest();
        for (int i = 0; i < pieces.count(c, t); ++i) {
            Piece other(c, t, i);
            if (other == p) continue;
            Square s = pieces.position(other);
            if (board.can_attack(t, s, to) && board.can_go(other, s, to)) {
                // If another piece of the same type can theoretically
                // attack the destination (fast answer by array lookup)
                // and can really go to this destination (not so fast
                // answer) then a disambiguation is needed
                stream << static_cast<char>('a' + m.orig_file());
                break;
            }
        }
    }

    // Capture
    if (m.is_capture()) {
        if (t == PAWN) stream << static_cast<char>('a' + m.orig_file());
        stream << "x";
    }

    // Destination
    stream << output_square(m.dest_file(), m.dest_rank());

    // Promotion
    if (m.is_promotion()) {
        stream << "=" << Piece(WHITE, m.promotion_type());
    }

    return stream.str();
}

std::string Game::output_square(File f, Rank r)
{
    std::ostringstream stream;
    stream << static_cast<char>('a' + f) << static_cast<char>('1' + r);
    return stream.str();
}

std::string get_stat(std::string title, double value, std::string unit = "")
{
    std::ostringstream stream;
    stream << std::left << "    " << std::setw(20) << title;
    int precision = (unit == "%" ? 2 : 0);
    stream << std::fixed << std::setprecision(precision) << value << unit;
    return stream.str();
}

std::string get_meta(double value, std::string unit)
{
    std::ostringstream stream;
    stream << " ("
           << std::fixed << std::setprecision(2) << value << unit
           << ")";
    return stream.str();
}

double get_percent(double a, double b)
{
    return 100 * a / b;
}

template <class T>
std::string print_table_stats(const HashTable<T>& table, int table_size)
{
    long zeros = 0;
    long ones = 0;
    for (int i = 0; i < table.size(); ++i) {
        Hash h = table.hash_at(i);
        if (!h) continue;
        std::bitset<64> b = h;
        int z = b.count();
        zeros += 64 - z;
        ones += z;
    }

    std::ostringstream stream;
    stream << get_stat("Table Size", table_size / 1024 / 1024, "Mb");
    stream << std::endl;

    stream << get_stat("Entries", table.size());
    stream << std::endl;

    stream << get_stat("Usage", table.usage());
    stream << get_meta(get_percent(table.usage(), table.size()), "%");
    stream << std::endl;

    stream << get_stat("0's", get_percent(zeros, 64 * table.usage()), "%");
    stream << std::endl;

    stream << get_stat("1's", get_percent(ones, 64 * table.usage()), "%");
    stream << std::endl;

    stream << get_stat("Lookups", table.nb_lookups());
    stream << std::endl;

    stream << get_stat("Hits", table.nb_hits());
    stream << get_meta(get_percent(table.nb_hits(),
                                   table.nb_lookups()), "%");
    stream << std::endl;

    stream << get_stat("Collisions", table.nb_collisions());
    stream << get_meta(get_percent(table.nb_collisions(),
                                   table.nb_lookups()), "%");
    stream << std::endl;

    stream << get_stat("Misses", table.nb_misses());
    stream << get_meta(get_percent(table.nb_misses(),
                                   table.nb_lookups()), "%");
    stream << std::endl;

    return stream.str();
}

void Game::print_tt_stats()
{
    std::cout << "Transposition Table usage:" << std::endl;
    std::cout << print_table_stats(tt, TT_SIZE) << std::endl;

    std::cout << "Material Table usage:" << std::endl;
    std::cout << print_table_stats(material_table, MT_SIZE) << std::endl;
}

std::string Game::debug_move(Move m)
{
    std::ostringstream stream;
    Color c = current_position().turn_color();
    stream << std::endl << board << std::endl
           << (c == WHITE ? "White" : "Black") << " to move" << std::endl
           << "m = " << output_move(m) << " (" << m << ")" << std::endl
           << "m is en passant: " << m.is_en_passant() << std::endl
           << "m is promotion: " << m.is_promotion() << std::endl
           << "m is legal: " << is_legal(m) << std::endl
           << std::hex << current_position().hash();
    return stream.str();
}
