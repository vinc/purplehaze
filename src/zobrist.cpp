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

#include "zobrist.h"

Zobrist::Zobrist() {
    generator.seed(); // Use Mersenne twister's default seed
    
    side_to_move = gen_hash();
    for (int c = 0; c < 2; ++c) {
        for (int t = 0; t < 7; ++t) {
            for (int s = 0; s < BOARD_SIZE; ++s) {
                pieces_positions[c][t][s] = gen_hash();
            }
        }
    }
    for (int i = 0; i < 4; ++i) {
        castle_rights[i] = gen_hash();
    }
    for (int i = 0; i < BOARD_SIZE; ++i) {
        en_passants[i] = gen_hash();
    }
}

// Generate a random Hash
Hash Zobrist::gen_hash() {
    Hash h = generator() ^ ((Hash) generator() << 32);
    return h;
}
