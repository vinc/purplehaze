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

#include "common.h"

#ifndef SEARCH_H
#define SEARCH_H

int quiescence_search(Board& board, Pieces& player, Pieces& opponent, int alpha, int beta);
int negamax_search(Board& board, Pieces& player, Pieces& opponent, int depth);
int alphabeta_search(Board& board, Pieces& player, Pieces& opponent, int alpha, int beta, int depth);
int principal_variation_search(Board& board, Pieces& player, Pieces& opponent, int alpha, int beta, int depth, bool null_move_pruning);

#endif /* !SEARCH_H */
