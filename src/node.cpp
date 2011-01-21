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

//#include <iostream>
//using namespace std;

#include "node.h"

Node::Node() {
    side_to_move = WHITE;
    half_move_counter = 0;
    score = 0;
    ply = 0;
    en_passant = OUT;
    castle_rights = 0; //0xF;
    //capture = Piece();
}

bool Node::can_castle(Color c, PieceType t) const {
    return castle_rights[2 * c + t - QUEEN];
}
void Node::set_castle_right(Color c, PieceType t, bool b) {
    castle_rights.set(2 * c + t - QUEEN, b);
}
