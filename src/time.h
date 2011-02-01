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

#ifndef TIME_H
#define TIME_H

#include <stdio.h>
#include <time.h>

class Time
{
    private:
	int allowed_moves;
	int allowed_time; // In centi-seconds

	clock_t starting_time;
	int allocated_time;
	int remaining_time;

	bool abort_search;
	int polling_interval;
	int last_poll_nodes_count;

    public:
	Time() : 
	    allowed_moves(40), allowed_time(24000), 
	    allocated_time(24000), remaining_time(24000),
	    abort_search(false), 
	    polling_interval(500000), last_poll_nodes_count(0)  {}
	Time(int moves, int time) : 
	    allowed_moves(moves), allowed_time(time), 
	    allocated_time(time), remaining_time(time) {}

	void set_remaining_time(int time) { remaining_time = time; };
	int get_allocated_time() const {
	    return allocated_time;
	};
	int get_elapsed_time() { 
	    return (100 * (clock() - starting_time)) / CLOCKS_PER_SEC;
	};
	void start_thinking(int ply);
	bool is_out_of_time();
	bool poll(int nodes_count);
};

#endif /* !TIME_H */
