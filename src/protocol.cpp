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

#include "protocol.h"

using namespace std;

void Protocol::new_game() {
    game.tt.clear();
    game.clear_killers();
    game.search_moves.clear();
}

bool Protocol::set_board(string fen) {
    // Test if fen is valid?
    
    // Load fen
    game.init(fen);

    return true;
}

/*
 * Set time control:
 * 'n' moves in 't' time with time in seconds.
 */
bool Protocol::set_time(int moves, int time) {
    game.time = Time(moves, time * 100);
    return true;
}

bool Protocol::set_remaining_time(int time) {
    game.time.set_remaining_time(time);
    return true;
}

bool Protocol::set_output_thinking(bool ot) {
    game.output_thinking = ot;
    return true;
}

Move Protocol::parse_move(string move) {
    Square from = Square(move[0] - 'a' + 16 * (move[1] - '1'));
    Square to = Square(move[2] - 'a' + 16 * (move[3] - '1'));
    if (game.board.is_out(from) || game.board.is_out(to)) return Move();
    Color c = game.current_position().get_turn_color();
    MoveType t = QUIET_MOVE;

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
        if (game.board.is_pawn_begin(c, from) &&
            to == Square(from + 2 * PAWN_PUSH_DIRS[c])) {
                return Move(from, to, DOUBLE_PAWN_PUSH);
        } else if (game.board.is_empty(to) && 
            to != Square(from + PAWN_PUSH_DIRS[c])) {
                return Move(from, to, EN_PASSANT);
        }
    }
    if (game.board.get_piece(from).get_type() == KING) {
        if (to == Square(from + RIGHT + RIGHT)) {
            return Move(from, to, KING_CASTLE);
        } else if (to == Square(from + LEFT + LEFT)) {
            return Move(from, to, QUEEN_CASTLE);
        }
    }

    // Capture
    if (!game.board.is_empty(to)) {
        assert((t == QUIET_MOVE) || 
               (KNIGHT_PROMOTION <= t && t <= QUEEN_PROMOTION));
        return Move(from, to, static_cast<MoveType>(t + CAPTURE));
    } else {
        return Move(from, to, t);
    }
}

bool Protocol::play_move(string move) {
    // Parse move
    Move m = parse_move(move);
    
    // Test legality
    if (!game.is_legal(m)) return false;
    
    // Play move
    game.make_move(m);

    // Put move to history
    history.push(m);

    return true;
}

bool Protocol::undo_move() {
    if (history.empty()) return false;
    Move m = history.top(); 
    history.pop();

    // Undo move
    game.undo_move(m);
    return true;
}

string Protocol::search_move(bool use_san_notation) {
    Move m = game.root(depth + 1);
    if (m.is_null()) {
        if (game.is_check(game.current_position().get_turn_color())) {
            return "LOST";
        }
        return "DRAW";
    } else {
        if (use_san_notation) {
            string res = game.output_move(m);
            play_move(m.to_string());
            if (game.is_check(game.current_position().get_turn_color())) {
                res += "+";
            }
            undo_move();
            return res;
        }
        return m.to_string();
    }
}
