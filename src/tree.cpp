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

#include "tree.h"

bool Tree::has_repetition_draw() {
    Node& current_node = tree[tree_top];
    if (current_node.get_halfmove() >= 99) return 0; // Fifty-move rule
    if (tree_top < 4) return false;
    Hash& pos = current_node.hash();
    int previous_halfmove = current_node.get_halfmove();
    for (int i = tree_top - 2; i >= 0; i -= 2) {
	if (tree[i].hash() == pos) return true; // Second repetition
	if (tree[i].get_halfmove() > previous_halfmove) { // Halfmove reseted
	    return false; // No previous repetition possible
	}
	previous_halfmove = tree[i].get_halfmove();
    }
    return false;
}
