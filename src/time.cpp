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
	int remaining_moves = allowed_moves - (ply % allowed_moves);
	allocated_time = remaining_time / remaining_moves;
    }
    else {
	allocated_time = allowed_time / allowed_moves;
    }
}

bool Time::is_out_of_time() {
    if (2 * get_elapsed_time() > get_allocated_time()) return true;
    else return false;
}

bool Time::poll(int nodes_count) {
    if (nodes_count - last_poll_nodes_count > polling_interval) {
	last_poll_nodes_count = nodes_count;
	abort_search = is_out_of_time();
    }
    return abort_search;
}
