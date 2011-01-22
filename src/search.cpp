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

#include <assert.h>
#include <iostream>
#include <list>
#include <vector>

#include "game.h"

using namespace std;

int Game::perft(int depth) {
    int nodes_count = 0;
    Color c = current_node().get_turn_color();
    if (depth == 0) return 1;
    Moves moves = movegen();
    for (moves.it = moves.begin(); moves.it != moves.end(); moves.it++) {
	//cout << *moves.it << endl;
	make_move(*moves.it);
	if (!is_check(c)) {
	    nodes_count += perft(depth - 1);
	}
	undo_move(*moves.it);
    }
    return nodes_count;
}
