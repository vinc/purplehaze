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

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <stack>

#include "common.h"
#include "game.h"

class Protocol
{
    protected:
        Game game;
        int depth;
        stack<Move> history;
        unsigned short verbosity;

    public:
        Protocol() : depth(MAX_PLY - 1), verbosity(0) {}

        void new_game();

        bool set_output_thinking(bool ot = true);

        bool set_board(string fen);

        bool set_time(int moves, int time);

        bool set_remaining_time(int time);

        bool play_move(string move);

        bool undo_move();

        string search_move(bool use_san_notation = false);

        Move parse_move(string move);

        void set_depth(int d) { depth = d; };

        void set_verbosity(int v = 0) { verbosity = v; };

        int get_verbosity() { return verbosity; };
};

#endif /* !PROTOCOL_H */
