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

#include <iostream>

#include "time.h"

void Time::start_thinking(int ply) {
    starting_time = clock();
    abort_search = false;
    last_poll_nodes_count = 0;
    polling_interval = 1000000;
    if (remaining_time != allocated_time) {
	int n = allowed_moves;
	int remaining_moves = n - (((ply + 1) / 2) % n);
	allocated_time = remaining_time / remaining_moves;
    }
    else {
	allocated_time = allowed_time / allowed_moves;
    }

    if (allocated_time > 3000) {
	coef_1 = 16; coef_2 = 15;
    }
    else if (allocated_time > 1000) {
	coef_1 = 4; coef_2 = 3;
    }
    else {
	coef_1 = 5; coef_2 = 3;
    }
}

bool Time::is_out_of_time() {
    if (coef_1 * get_elapsed_time() > coef_2 * get_allocated_time()) {
	return true;
    }
    else return false;
}

bool Time::poll(int nodes_count) {
    if (nodes_count - last_poll_nodes_count > polling_interval) {
	last_poll_nodes_count = nodes_count;
	abort_search = is_out_of_time();
    }
    return abort_search;
}
