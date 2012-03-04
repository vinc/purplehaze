/* PurpleHaze 2.0.0
 * Copyright (C) 2007-2011  Vincent Ollivier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <bitset>

#include "game.h"

static const int WIDE = 10;

void Game::print_thinking_header() {
    if (!output_thinking) return;
    cout << setw(4) << "ply";
    cout << setw(WIDE - 1) << "score";
    cout << setw(WIDE) << "time";
    cout << setw(WIDE + 3) << "nodes";
    cout << setw(WIDE) << "pv";
    cout << endl;
}

void Game::print_thinking(int depth, int score, Move m) {
    if (!output_thinking) return;
    cout << setw(4) << depth;
    cout << setw(WIDE - 1) << score;
    cout << setw(WIDE) << time.get_elapsed_time();
    cout << setw(WIDE + 3) << nodes_count;
    cout << setw(WIDE - 3) << " ";
    int ply = current_position().get_ply();

    if (current_position().get_turn_color() == BLACK) {
        cout << " " << 1 + (ply / 2) << ". ...";
    }

    assert(is_legal(m) || assert_msg(debug_move(m)));
    cout << output_pv(depth, score, m) << endl;
}

bool is_mate(int score) {
    if ((score < -INF + MAX_PLY) || (INF - MAX_PLY < score)) return true;
    return false;
}

string Game::output_pv(int depth, int score, Move m) {
    ostringstream stream;
    stream << " ";
    int ply = current_position().get_ply();
    if (current_position().get_turn_color() == WHITE) {
        stream << 1 + (ply / 2) << ". ";
    }
    stream << output_move(m);

    make_move(m);

    bool is_in_check = is_check(current_position().get_turn_color());

    // Find next move in TT
    bool is_empty;
    Transposition trans = tt.lookup(current_position().hash(), &is_empty);
    Move move = trans.get_best_move();
    if (depth > 0 && is_legal(move) && trans.get_bound() < 3) {
        if (is_in_check) stream << "+"; // Check
        stream << output_pv(depth - 1, trans.get_value(), move);
    } else if (move.is_null() && is_mate(score)) {
        if (is_in_check) stream << "#"; // Mate
    } else if (is_in_check) {
        stream << "+"; // Cut-off
    }

    undo_move(m);
    return stream.str();
}

string Game::output_move(Move m) {
    ostringstream stream;

    // Castling
    if (m.is_castle()) {
        if (m.get_castle_side() == QUEEN) stream << "O-";
        return stream.str() + "O-O";
    }

    // Type of piece
    Square from = m.get_orig();
    Piece p = board.get_piece(from);
    PieceType t = p.get_type();
    if (t > PAWN) stream << Piece(WHITE, t); // Upper case

    // Disambiguation
    if (t != PAWN) {
        Color c = p.get_color();
        Square to = m.get_dest();
        for (int i = 0; i < pieces.get_nb_pieces(c, t); ++i) {
            Piece other(c, t, i);
            if (other == p) continue;
            Square s = pieces.get_position(other);
            if (board.can_attack(t, s, to) && board.can_go(other, s, to)) {
                // If another piece of the same type can theoretically
                // attack the destination (fast answer by array lookup)
                // and can really go to this destination (not so fast
                // answer) then a disambiguation is needed
                stream << static_cast<char>('a' + m.get_orig_file());
                break;
            }
        }
    }

    // Capture
    if (m.is_capture()) {
        if (t == PAWN) stream << static_cast<char>('a' + m.get_orig_file());
        stream << "x";
    }

    // Destination
    stream << output_square(m.get_dest_file(), m.get_dest_rank());

    // Promotion
    if (m.is_promotion()) {
        stream << "=" << Piece(WHITE, m.get_promotion_type());
    }

    return stream.str();
}

string Game::output_square(File f, Rank r) {
    ostringstream stream;
    stream << static_cast<char>('a' + f) << static_cast<char>('1' + r);
    return stream.str();
}

string get_stat(string title, double value, string unit = "") {
    ostringstream stream;
    stream << left << "    " << setw(20) << title;
    int precision = (unit == "%" ? 2 : 0);
    stream << fixed << setprecision(precision) << value << unit;
    return stream.str();
}

string get_meta(double value, string unit) {
    ostringstream stream;
    stream << " (" << fixed << setprecision(2) << value << unit << ")";
    return stream.str();
}

double get_percent(double a, double b) {
    return 100 * a / b;
}

template <class T>
string print_table_stats(HashTable<T>& table, int table_size) {
    long zeros = 0;
    long ones = 0;
    for (int i = 0; i < table.size(); ++i) {
        Hash h = table.get_hash_at(i);
        if (!h) continue;
        bitset<64> b = h;
        int z = b.count();
        zeros += 64 - z;
        ones += z;
    }

    ostringstream stream;
    stream << get_stat("Table Size", table_size / 1024 / 1024, "Mb");
    stream << endl;

    stream << get_stat("Entries", table.size());
    stream << endl;

    stream << get_stat("Usage", table.get_usage());
    stream << get_meta(get_percent(table.get_usage(), table.size()), "%");
    stream << endl;

    stream << get_stat("0's", get_percent(zeros, 64 * table.get_usage()), "%");
    stream << endl;

    stream << get_stat("1's", get_percent(ones, 64 * table.get_usage()), "%");
    stream << endl;

    stream << get_stat("Lookups", table.get_nb_lookups());
    stream << endl;

    stream << get_stat("Hits", table.get_nb_hits());
    stream << get_meta(get_percent(table.get_nb_hits(),
                                   table.get_nb_lookups()), "%");
    stream << endl;

    stream << get_stat("Collisions", table.get_nb_collisions());
    stream << get_meta(get_percent(table.get_nb_collisions(),
                                   table.get_nb_lookups()), "%");
    stream << endl;

    stream << get_stat("Misses", table.get_nb_misses());
    stream << get_meta(get_percent(table.get_nb_misses(),
                                   table.get_nb_lookups()), "%");
    stream << endl;

    return stream.str();
}

void Game::print_tt_stats() {
    cout << "Transposition Table usage:" << endl;
    cout << print_table_stats(tt, TT_SIZE) << endl;

    cout << "Material Table usage:" << endl;
    cout << print_table_stats(material_table, MT_SIZE) << endl;
}

string Game::debug_move(Move m) {
    ostringstream stream;
    Color c = current_position().get_turn_color();
    stream << endl << board << endl <<
              (c == WHITE ? "White" : "Black") << " to move" << endl <<
              "m = " << output_move(m) << " (" << m << ")" << endl <<
              "m is en passant: " << m.is_en_passant() << endl <<
              "m is promotion: " << m.is_promotion() << endl <<
              "m is legal: " << is_legal(m) << endl <<
              hex << current_position().hash();
    return stream.str();
}
