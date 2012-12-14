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

#include <bitset>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include "game.h"

static const int WIDE = 10;

void Game::print_thinking_header()
{
    if (!output_thinking) {
        return;
    }
    std::cout << std::setw(4) << "ply"
              << std::setw(WIDE - 1) << "score"
              << std::setw(WIDE) << "time"
              << std::setw(WIDE + 3) << "nodes"
              << std::setw(WIDE) << "pv"
              << std::endl;
}

void Game::print_thinking(int depth, int score, Move m)
{
    if (!output_thinking) {
        return;
    }
    std::cout << std::setw(4) << depth
              << std::setw(WIDE - 1) << score
              << std::setw(WIDE) << time.elapsed()
              << std::setw(WIDE + 3) << nodes_count
              << std::setw(WIDE - 3) << " ";
    const int ply = positions.ply();

    if (positions.current().side() == BLACK) {
        std::cout << " " << 1 + (ply / 2) << ". ...";
    }

    assert(is_legal(m) || assert_msg(debug_move(m)));
    std::cout << output_pv(depth, score, m) << std::endl;
}

static bool is_mate(int score)
{
    return ((score < -INF + MAX_PLY) || (INF - MAX_PLY < score));
}

std::string Game::output_pv(int depth, int score, Move m)
{
    std::ostringstream stream;
    stream << " ";
    const int ply = positions.ply();
    if (positions.current().side() == WHITE) {
        stream << 1 + (ply / 2) << ". ";
    }
    stream << output_move(m);

    make_move(m); // Update nodes_count
    --nodes_count;

    bool is_in_check = is_check(positions.current().side());

    // Find next move in TT
    bool is_empty;
    Transposition trans = tt.lookup(positions.current().hash(), &is_empty);
    Move move = trans.best_move();
    if (depth > 0 && is_legal(move) && trans.bound() < 3) {
        if (is_in_check) {
            stream << "+"; // Check
        }
        stream << output_pv(depth - 1, trans.value(), move);
    } else if (move.is_null() && is_mate(score)) {
        if (is_in_check) {
            stream << "#"; // Mate
        }
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
        if (m.castle_side() == QUEEN) {
            stream << "O-";
        }
        return stream.str() + "O-O";
    }

    // Type of piece
    Square from = m.orig();
    Piece p = board[from];
    PieceType t = p.type();
    if (t > PAWN) {
        stream << Piece(WHITE, t); // Upper case
    }

    // Disambiguation
    if (t != PAWN) {
        Color c = p.color();
        Square to = m.dest();
        for (int i = 0; i < pieces.count(c, t); ++i) {
            Piece other(c, t, i);
            if (other == p) {
                continue;
            }
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
        if (t == PAWN) {
            stream << static_cast<char>('a' + m.orig_file());
        }
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

template <class T>
std::string Game::output_hashtable_stats(const HashTable<T>& table)
{
    static const std::string names[] = {
        "Size",
        "Usage",
        "Lookups",
        "Hits",
        "Misses",
        "Collisions"
    };
    const long stats[] = {
        table.size(),
        table.usage(),
        table.stats.hits + table.stats.misses,
        table.stats.hits,
        table.stats.misses,
        table.stats.collisions
    };

    int width = 0;
    for (const std::string& name : names) {
        width = std::max(width, static_cast<int>(name.length()) + 2);
    }
    std::ostringstream stream;
    for (int i = 0; i < 6; ++i) {
        stream << std::left << "   " << std::setw(width) << names[i]
               << std::fixed << std::setprecision(2) << stats[i];
        if (names[i] == "Usage" || i > 2) {
            const long n = (i > 2 ? stats[2] : table.size());
            const double v = 100.0 * stats[i] / n;
            stream << " (" << std::fixed << std::setprecision(2) << v << "%)";
        }
        if (i < 5) {
            stream << std::endl;
        }
    }
    return stream.str();
}

std::string Game::debug_move(Move m)
{
    std::ostringstream stream;
    Color c = positions.current().side();
    stream << std::endl << board << std::endl
           << (c == WHITE ? "White" : "Black") << " to move" << std::endl
           << "m = " << output_move(m) << " (" << m << ")" << std::endl
           << "m is en passant: " << m.is_en_passant() << std::endl
           << "m is promotion: " << m.is_promotion() << std::endl
           << "m is legal: " << is_legal(m) << std::endl
           << std::hex << positions.current().hash();
    return stream.str();
}

// Define types used by template to avoid linker errors
template std::string Game::output_hashtable_stats(HashTable<Transposition> const&);
template std::string Game::output_hashtable_stats(HashTable<Material> const&);
