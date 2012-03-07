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

#include "pieces.h"

Pieces::Pieces() {
    /*
     * Unused array entries should not be expected to remain equal to OUT
     * during the game, but rather to the position of the previous piece
     * pointing to the entry. But currently we use an extra instruction to
     * set it to OUT whenever a piece is removed.
     *
     * Great care should be taken when updating the indexes counters
     * to remain within 0 and 9 because no verifications are done in
     * this class.
     */

    for (int i = 0; i < 2; ++i) {
        total[i] = 0;
        for (int j = 0; j < 7; ++j) {
            nb_pieces[i][j] = 0;
            for (int k = 0; k < 9; ++k) {
                positions[i][j][k] = OUT;
            }
        }
    }
}
