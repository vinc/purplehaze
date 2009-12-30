/*  PurpleHaze 1.0
    Copyright (C) 2007-2009  Vincent Ollivier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MOVEGEN_H
#define MOVEGEN_H

using namespace std;

void movegen_leapers(Board& board, Moves& moves, Piece* ptr_piece, const MoveOrientation direction[], const int nb_moves, bool capture_only);
void movegen_sliders(Board& board, Moves& moves, Piece* ptr_piece, const MoveOrientation direction[], const int nb_moves, bool capture_only);
void movegen_capture(Board& board, Moves& moves, Piece* ptr_piece, Color color, Square from, Square to);

Moves movegen(Board& board, Pieces& pieces, bool capture_only);

void make_move(Board& board, Move& move);
void unmake_move(Board& board, Move& move);

#endif /* !MOVEGEN_H */
