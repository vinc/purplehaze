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
	// Set the time to play the game
	int allowed_moves;
	int allowed_time; // In centi-seconds

	// Set the time to play a move
	clock_t starting_time;
	int allocated_time; // Calculated
	int remaining_time; // Given by protocols like Xboard
	int coef_1, coef_2;

	int polling_interval;
	int last_poll_nodes_count;
	bool abort_search;

    public:
	Time() : 
	    allowed_moves(40), allowed_time(24000), 
	    allocated_time(allowed_time / allowed_moves), 
	    remaining_time(allowed_time / allowed_moves),
	    polling_interval(500000), last_poll_nodes_count(0),
	    abort_search(false) {} 
	Time(int moves, int time) : 
	    allowed_moves(moves), allowed_time(time), 
	    allocated_time(allowed_time / allowed_moves), 
	    remaining_time(allowed_time / allowed_moves),
	    polling_interval(500000), last_poll_nodes_count(0),
	    abort_search(false) {} 

	void set_polling_interval(int nodes) { polling_interval = nodes; };
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
