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

#include "positions.h"

bool Positions::is_draw()
{
    const int hm = stack[size].halfmove();

    // Fifty-move rule
    if (hm >= 100) {
        return true;
    }

    // Threefold repetition
    for (int i = size - 2, n = std::max(size - hm, 0); i >= n; i -= 2) {
        if (stack[i].hash() == stack[size].hash()) {
            return true; // Second repetition
        }
    }

    return false;
}
