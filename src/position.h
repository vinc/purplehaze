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

#ifndef NODE_H
#define NODE_H

#include <bitset>

#include "common.h"
#include "piece.h"
#include "zobrist.h"

class Position
{
private:
    Hash zobrist_hash;
    Hash material_zobrist_hash;
    Piece captured_piece;
    std::bitset<4> castle_rights;
    std::bitset<2> castled;
    Square en_passant_square;
    uint8_t halfmove_counter;
    bool null_move_right;
    Color side_to_move;

public:
    Position() :
        en_passant_square(OUT),
        halfmove_counter(0),
        null_move_right(true),
        side_to_move(WHITE) {}

    Hash& hash() {
        return zobrist_hash;
    }
    Hash& material_hash() {
        return material_zobrist_hash;
    }
    Color side() const {
        return side_to_move;
    }
    void change_side() {
        side_to_move = !side_to_move;
    }
    uint8_t halfmove() const {
        return halfmove_counter;
    }
    void set_halfmove(uint8_t i) {
        halfmove_counter = i;
    }
    void inc_halfmove() {
        ++halfmove_counter;
    }
    void reset_halfmove() {
        halfmove_counter = 0;
    }
    Square en_passant() const {
        return en_passant_square;
    }
    void set_en_passant(Square ep) {
        en_passant_square = ep;
    }
    Piece capture() const {
        return captured_piece;
    }
    void set_capture(Piece p) {
        captured_piece = p;
    }
    bool can_null_move() const {
        return null_move_right;
    }
    void set_null_move_right(bool b) {
        null_move_right = b;
    }
    bool can_castle(Color c, PieceType t) const {
        return castle_rights[2 * c + t - QUEEN];
    }
    void set_castle_right(Color c, PieceType t, bool b = true) {
        castle_rights[2 * c + t - QUEEN] = b;
    }
    bool has_castled(Color c) const {
        return castled[c];
    }
    void set_has_castled(Color c, bool b = true) {
        castled[c] = b;
    }
};

#endif /* !NODE_H */
