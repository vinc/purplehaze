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

#include "protocol.h"

Protocol::Protocol() {
    
}
void Protocol::new_game(){
    game = Game();
}

bool Protocol::set_board(string fen){
    // Test if fen is valid?
    
    // Load fen
    game.init(fen);

    return true;
}

Move Protocol::parse_move(string move) {
    Square from = Square((int(move[0]) - 'a' + 16 * int(move[1]) - '1'));
    Square to = Square((int(move[2]) - 'a' + 16 * int(move[3]) - '1'));
    Color c = game.current_node().get_turn_color();
    MoveType t;

    if (move.size() == 5) { // Promotion
	switch (move[4]) {
	    case 'n': t = KNIGHT_PROMOTION; break;
	    case 'b': t = BISHOP_PROMOTION; break;
	    case 'r': t = ROOK_PROMOTION; break;
	    case 'q': t = QUEEN_PROMOTION; break;
	    default: return Move();
	}
    }
    if (game.board.get_piece(from).get_type() == PAWN) {
	if (game.board.is_empty(to) && 
	    to != Square(from + PAWN_PUSH_DIRS[c])) {
		return Move(from, to, EN_PASSANT);
	}
	else if (game.board.is_pawn_begin(c, from) &&
	    to == Square(from + 2 * PAWN_PUSH_DIRS[c])) {
		return Move(from, to, DOUBLE_PAWN_PUSH);
	}
    }
    if (game.board.get_piece(from).get_type() == KING) {
	if (to == Square(from + RIGHT + RIGHT)) {
	    return Move(from, to, KING_CASTLE);
	}
	else if (to == Square(from + LEFT + LEFT)) {
	    return Move(from, to, QUEEN_CASTLE);
	}
    }
    if (!game.board.is_empty(to)) { // Capture
	assert((t == QUIET_MOVE) || 
	       (t >= KNIGHT_PROMOTION && t <= QUEEN_PROMOTION));
	return Move(from, to, MoveType(t + CAPTURE));
    }
    else return Move(from, to, QUIET_MOVE);
}

bool Protocol::play_move(string move){
    // Parse move
    Move m = parse_move(move);

    // Test legality
    if (!game.is_legal_move(m)) return false;
    
    // Play move
    game.make_move(m);
    return true;
}

bool Protocol::undo_move(string move){
    // Parse move
    Move m = parse_move(move);

    // TODO Check if it was the last move
    //if (game.history.top() != m) return false;

    // Undo move
    game.undo_move(m);
    return true;
}

string Protocol::search_move(bool use_san_notation){
    Move m = game.root(5);
    if (m.is_null()) {
	switch (game.eval()) {
	    case  INF: return  "WIN";
	    case -INF: return "LOSS"; 
	    default  : return "DRAW";
	}
    }
    else return m.to_string();
}
