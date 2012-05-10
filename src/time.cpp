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
    start = clock();
    abort_search = false;
    polling.interval = 1000000;
    polling.previous = 0;

    // Compute time per move
    if (remaining != time_per_move) {
        unsigned int n = level_moves;
        n = n - (((ply + 1) / 2) % n);
        assert(n > 0);
        time_per_move = remaining / n;
    } else {
        assert(level_moves > 0);
        time_per_move = level_time / level_moves;
    }

    // Compute coefs
    if (time_per_move > 3000) {
        coef_1 = 16; coef_2 = 15;
    } else if (time_per_move > 1000) {
        coef_1 = 4; coef_2 = 3;
    } else {
        coef_1 = 5; coef_2 = 3;
    }
}

bool Time::is_out_of_time() const
{
    return coef_1 * elapsed() > coef_2 * allocated();
}

bool Time::poll(const unsigned int node_count)
{
    // Avoid wasting time by calling 'is_out_of_time()' too frequently
    if (node_count - polling.previous > polling.interval) {
        polling.previous = node_count;
        abort_search = is_out_of_time();
    }
    return abort_search;
}
