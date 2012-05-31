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

#ifndef TIME_H
#define TIME_H

#include <stdio.h>
#include <time.h>

class Time
{
    private:
        struct Polling {
            unsigned int interval;
            unsigned int previous;
            Polling() : interval(50000), previous(0) {}
        } polling;

        // Set the time to play the game
        unsigned int level_moves;
        unsigned int level_time; // In centi-seconds

        // Set the time to play a move
        clock_t start;
        unsigned long long int time_per_move; // Calculated
        unsigned int remaining; // Given by protocols like Xboard
        int coef_1, coef_2;

        bool abort_search;

        bool is_out_of_time() const;

    public:
        Time(unsigned int moves = 40, unsigned int time = 24000) :
            level_moves(moves), level_time(time),
            time_per_move(level_time / level_moves),
            remaining(time_per_move),
            abort_search(false)
            {}

        void set_polling_interval(const unsigned int nodes) {
            polling.interval = nodes;
        };
        void set_remaining(const unsigned int time) {
            remaining = time;
        };
        unsigned long long int allocated() const {
            return time_per_move;
        };
        unsigned long long int elapsed() const {
            unsigned long long int clocks = clock() - start;
            return 100 * clocks / CLOCKS_PER_SEC;
        };
        void abort() {
            abort_search = true;
        };
        void start_thinking(const unsigned int ply);
        bool poll(const unsigned int node_count);
};

#endif /* !TIME_H */
