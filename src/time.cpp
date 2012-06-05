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

#include "time.h"

void Time::start_thinking(const unsigned int ply)
{
    // Reset variables
    start = std::clock();
    abort_search = false;
    polling.interval = 1000000;
    polling.previous = 0;

    clock.moves = ((ply / 2) % level.moves) + 1; // Used by allocated()

    // Compute ratio
    const int time_per_move = allocated();
    if (time_per_move < 4 * 4) {
        ratio = 4;
    } else if (time_per_move < 8 * 8) {
        ratio = 8;
    } else {
        ratio = 16;
    }
    if (clock.moves == level.moves) { // Last move
        ratio /= 2;
    }

    /*
    std::cout << "# start_thinking(" << ply << ")" << std::endl;
    std::cout << "#\tlevel = {" << level.moves << ", " << level.time << "}" << std::endl;
    std::cout << "#\tclock = {" << clock.moves << ", " << clock.time << "}" << std::endl;
    std::cout << "#\tratio = " << ratio << std::endl;
    std::cout << "#\ttpm   = " << time_per_move << std::endl;
    */
}

inline bool Time::is_out_of_time() const
{
    return ratio * elapsed() > (ratio - 1) * allocated();
}

bool Time::poll(const unsigned int node_count)
{
    // Avoid wasting time by calling 'is_out_of_time()' too frequently
    if (node_count - polling.previous > polling.interval) {
        polling.previous = node_count;
        abort_search = abort_search || is_out_of_time();
    }
    return abort_search;
}
