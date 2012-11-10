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

#ifndef TIME_H
#define TIME_H

#include <cassert>
#include <ctime>

class Time
{
    private:
        struct Polling {
            unsigned int interval;
            unsigned int previous;
            Polling() : interval(50000), previous(0) {}
        } polling;

        struct Clock {
            unsigned int moves;
            unsigned int time; // Centiseconds
            Clock(int m, int t) : moves(m), time(t) {}
        } level, clock;

        unsigned int ratio;
        clock_t start;
        bool abort_search;

        bool is_out_of_time() const;

    public:
        Time(const int moves = 40, const int time = 5 * 60 * 100) :
            level(moves, time),
            clock(1, time),
            abort_search(false)
            {}

        void set_polling_interval(const unsigned int nodes) {
            polling.interval = nodes;
        };
        void set_remaining(const unsigned int time) {
            clock.time = time;
        };
        unsigned int allocated() const {
            assert(level.moves >= clock.moves);
            const unsigned int moves = level.moves - clock.moves + 1;
            return clock.time / moves;
        };
        unsigned long long int elapsed() const {
            const unsigned long long int clocks = std::clock() - start;
            return 100 * clocks / CLOCKS_PER_SEC;
        };
        void abort() {
            abort_search = true;
        };
        void start_thinking(const unsigned int ply);
        bool poll(const unsigned int node_count);
};

#endif /* !TIME_H */
