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

#include "piece.h"
#include "move.h"
#include "zobrist.h"
#include "board.h"
#include "movegen.h"
#include "attack.h"

/*
 * Move generator for Knight and King
 */
void movegen_leapers(Board& board, Moves& moves, Piece* ptr_piece, const MoveOrientation direction[], const int nb_moves, bool capture_only) {
	Square from = ptr_piece->get_position(), to;
	Color color = ptr_piece->get_color();
	for (int i = 0; i < nb_moves; ++i) {
		to = Square(from + direction[i]);
		if (!capture_only && !board.is_off_the_board(to) && !board.get_ptr_piece(to)) {
			Move m(ptr_piece, from, to);
			moves.insert(m, BACK);
		}
		movegen_capture(board, moves, ptr_piece, color, from, to);
	}
}

/*
 * Move generator for Bishop, Rook and Queen
 */
void movegen_sliders(Board& board, Moves& moves, Piece* ptr_piece, const MoveOrientation direction[], const int nb_moves, bool capture_only) {
	Square from = ptr_piece->get_position(), to;
	Color color = ptr_piece->get_color();
	for (int i = 0; i < nb_moves; ++i) {
		to = Square(from + direction[i]);
		while (!board.is_off_the_board(to) && !board.get_ptr_piece(to)) {
			if (!capture_only) {
				Move m(ptr_piece, from, to);
				moves.insert(m, BACK);
			}
			to = Square(to + direction[i]);
		}
		movegen_capture(board, moves, ptr_piece, color, from, to);
	}
}

/*
 * Captures generator
 */
void movegen_capture(Board& board, Moves& moves, Piece* ptr_piece, Color color, Square from, Square to) {
	Piece* ptr_capture = board.get_ptr_piece(to);
	if (!board.is_off_the_board(to) && ptr_capture) {
		if (ptr_capture->get_color() != color) {
			
			// Capture + Pawn promotion
			if (ptr_piece->get_type() == PAWN &&
				((color == WHITE && board.get_rank(to) == 7) || 
				(color == BLACK && board.get_rank(to) == 0))
				) {
				Move knight(ptr_piece, from, to, ptr_capture, KNIGHT);
				moves.insert(knight, FRONT);
				Move bishop(ptr_piece, from, to, ptr_capture, BISHOP);
				moves.insert(bishop, FRONT);
				Move rook(ptr_piece, from, to, ptr_capture, ROOK);
				moves.insert(rook, FRONT);
				Move queen(ptr_piece, from, to, ptr_capture, QUEEN);
				moves.insert(queen, FRONT);
			}
			
			// Capture
			else {
				Move m(ptr_piece, from, to, ptr_capture);
				moves.insert(m, FRONT);
			}
		}
	}
}

/*
 * Generate a collection of all pseudo-moves and captures for a collection of pieces
 */				
Moves movegen(Board& board, Pieces& pieces, bool capture_only) {
	Moves moves;
	//capture_only = false;
	for (pieces.iterator = pieces.begin(); pieces.iterator != pieces.end(); pieces.iterator++) {
		Piece* ptr_piece = pieces.get_ptr_piece();
		Square from = ptr_piece->get_position(), to;
		if (from != OUT) {	
			Color color = ptr_piece->get_color();
			switch (ptr_piece->get_type()) {
				case PAWN:
					// Standard Move
					if (!capture_only) {
						to = (color == WHITE) ? Square(from + UP) : Square(from + DOWN);
						if (!board.is_off_the_board(to) && !board.get_ptr_piece(to)) {
						
							// Promotion
							if ((color == WHITE && board.get_rank(to) == 7) 
								 || (color == BLACK && board.get_rank(to) == 0)) {
								Move knight(ptr_piece, from, to, KNIGHT);
								moves.insert(knight, FRONT);
								Move bishop(ptr_piece, from, to, BISHOP);
								moves.insert(bishop, FRONT);
								Move rook(ptr_piece, from, to, ROOK);
								moves.insert(rook, FRONT);
								Move queen(ptr_piece, from, to, QUEEN);
								moves.insert(queen, FRONT);
							}
							else {
								Move m(ptr_piece, from, to);
								moves.insert(m, BACK);
							}
				
							// First move rule
							Square ep = to;
							if ((color == WHITE && board.get_rank(from) == 1) 
							 || (color == BLACK && board.get_rank(from) == 6)) {
								to = (color == WHITE) ? Square(to + UP) : Square(to + DOWN);
								if (!board.is_off_the_board(to) && !board.get_ptr_piece(to)) {
									// true for en passant
									//cout << "ep: " << ep << endl;
									Move m(ptr_piece, from, to, ep);
									moves.insert(m, BACK);
								
									//TODO flag the pawn as vulnerable to en passant
								}
							}
						}
					}
				
					// Capture right
					to = (color == WHITE) ? Square(from + UP_RIGHT) : Square(from + DOWN_RIGHT);
					movegen_capture(board, moves, ptr_piece, color, from, to);
					
					// Capture left
					to = (color == WHITE) ? Square(from + UP_LEFT) : Square(from + DOWN_LEFT);
					movegen_capture(board, moves, ptr_piece, color, from, to);
					
					// En passant
					if (!capture_only) {
						if ((color == WHITE && board.get_rank(from) == 4) || (color == BLACK && board.get_rank(from) == 3)) {
							Piece* ptr_right = board.get_ptr_piece(Square(from + RIGHT));
							if (ptr_right
								&& ptr_right->get_type() == PAWN
								&& ptr_right->get_color() != color
								&& ptr_right->get_nb_moves() == 1) {
								//board.print();
								to = (color == WHITE) ? Square(from + UP_RIGHT) : Square(from + DOWN_RIGHT);
								//cout << "board: " << board.get_en_passant() << " to: " << to << endl;
								if (board.get_en_passant() == to) {
									Move m(ptr_piece, from, to, ptr_right, EN_PASSANT);
									moves.insert(m, FRONT);	
								}
							}
												
							Piece* ptr_left = board.get_ptr_piece(Square(from + LEFT));
							if(ptr_left
								&& ptr_left->get_type() == PAWN
								&& ptr_left->get_color() != color
								&& ptr_left->get_nb_moves() == 1) {
								//board.print();
								to = (color == WHITE) ? Square(from + UP_LEFT) : Square(from + DOWN_LEFT);
								//cout << "board: " << board.get_en_passant() << " to: " << to << endl;
								if (board.get_en_passant() == to) {
									Move m(ptr_piece, from, to, ptr_left, EN_PASSANT);
									moves.insert(m, FRONT);
								}
							}
						}
					}
					
					break;
					
				case KNIGHT:
					movegen_leapers(board, moves, ptr_piece, KNIGHT_MOVES, NB_KNIGHT_MOVES, capture_only);
					break;
					
				case KING:
					// Checkmates detection
					if (ptr_piece->get_position() == OUT) {
						Moves mates;
						return mates;
					}
					
					movegen_leapers(board, moves, ptr_piece, KING_MOVES, NB_KING_MOVES, capture_only);
					
					
					// Castle
					if (!capture_only) {
						Piece* ptr_rook;
						switch (color) {
							case WHITE:
								if (ptr_piece->get_nb_moves() == 0 &&
									is_attacked_by(board, E1, BLACK).size() == 0
									){
								
									// King castle
									ptr_rook = board.get_ptr_piece(H1);
									if (ptr_rook &&
										ptr_rook->get_type() == ROOK &&
										ptr_rook->get_nb_moves() == 0 &&
										!board.get_ptr_piece(F1) &&
										is_attacked_by(board, F1, BLACK).size() == 0 &&
										!board.get_ptr_piece(G1) &&
										is_attacked_by(board, G1, BLACK).size() == 0
										) {
										Move m(ptr_piece, E1, G1, ptr_rook, CASTLE);
										moves.insert(m, FRONT);
									}
								
									// Queen castle
									ptr_rook = board.get_ptr_piece(A1);
									if (ptr_rook &&
										ptr_rook->get_type() == ROOK &&
										ptr_rook->get_nb_moves() == 0 &&
										!board.get_ptr_piece(D1) &&
										is_attacked_by(board, D1, BLACK).size() == 0 &&
										!board.get_ptr_piece(C1) &&
										is_attacked_by(board, C1, BLACK).size() == 0 &&
										!board.get_ptr_piece(B1)
									
										) {
										Move m(ptr_piece, E1, C1, ptr_rook, CASTLE);
										moves.insert(m, FRONT);
									}
								}
								break;
							case BLACK:
								if (ptr_piece->get_nb_moves() == 0 &&
									is_attacked_by(board, E8, WHITE).size() == 0
									){
								
									// King castle
									ptr_rook = board.get_ptr_piece(H8);
									if (ptr_rook &&
										ptr_rook->get_type() == ROOK &&
										ptr_rook->get_nb_moves() == 0 &&
										!board.get_ptr_piece(F8) &&
										is_attacked_by(board, F8, WHITE).size() == 0 &&
										!board.get_ptr_piece(G8) &&
										is_attacked_by(board, G8, WHITE).size() == 0
										) {
										Move m(ptr_piece, E8, G8, ptr_rook, CASTLE);
										moves.insert(m, FRONT);
									}
								
									// Queen castle
									ptr_rook = board.get_ptr_piece(A8);
									if (ptr_rook &&
										ptr_rook->get_type() == ROOK &&
										ptr_rook->get_nb_moves() == 0 &&
										!board.get_ptr_piece(D8) &&
										is_attacked_by(board, D8, WHITE).size() == 0 &&
										!board.get_ptr_piece(C8) &&
										is_attacked_by(board, C8, WHITE).size() == 0 &&
										!board.get_ptr_piece(B8)
										) {
										Move m(ptr_piece, E8, C8, ptr_rook, CASTLE);
										moves.insert(m, FRONT);
									}
								}
								break;
							default:
								cout << "Error!";
								break;
						}
					}
					//*/
					
					break;
					
				case BISHOP:
					movegen_sliders(board, moves, ptr_piece, BISHOP_MOVES, NB_BISHOP_MOVES, capture_only);
					break;
					
				case ROOK:
					movegen_sliders(board, moves, ptr_piece, ROOK_MOVES, NB_ROOK_MOVES, capture_only);
					break;
					
				case QUEEN:
					movegen_sliders(board, moves, ptr_piece, QUEEN_MOVES, NB_QUEEN_MOVES, capture_only);
					break;
					
				case UNDEF_PIECE_TYPE:
					break;
			}
		}
	}
	//moves.sort();
	return moves;
}


void make_move(Board& board, Move& move) {
	
	// If it is not a null move
	if (move.get_type() != UNDEF_MOVE_TYPE) {
	
		bool is_repetition = true; // for the 50 repetitions

		board.change_turn_color();
		
		//cout << "DEBUG: Zobrist before making " << move << " = \t" << board.zobrist << endl;
		board.zobrist.add_move(move);
		//cout << "DEBUG: Zobrist after making " << move << " = \t" << board.zobrist << endl;
	
		move.get_ptr_piece()->inc_nb_moves();
		switch (move.get_type()) {
			case MOVE:
				board.set_ptr_piece(move.get_ptr_piece(), move.get_to());
				if (move.get_ptr_piece()->get_type() == PAWN) {
					is_repetition = false;
				}
				break;
			case EN_PASSANT:
				board.set_ptr_piece(move.get_ptr_captured_piece(), OUT);
				board.set_ptr_piece(move.get_ptr_piece(), move.get_to());
				is_repetition = false;
				break;
			case CAPTURE:
				board.set_ptr_piece(move.get_ptr_captured_piece(), OUT);
				board.set_ptr_piece(move.get_ptr_piece(), move.get_to());
				is_repetition = false;
				break;
			case CASTLE:
				board.set_ptr_piece(move.get_ptr_piece(), move.get_to());
				Square s;
				switch (move.get_to()) {
					case G1: s = F1; break;
					case C1: s = D1; break;
					case G8: s = F8; break;
					case C8: s = D8; break;
					default: s = OUT; break;
				}
				board.set_ptr_piece(move.get_ptr_captured_piece(), s);
				move.get_ptr_captured_piece()->inc_nb_moves();
				Color color;
				color = (move.get_ptr_piece()->get_color() == WHITE) ? WHITE : BLACK;
				board.set_castling_right(color, false);
				break;
			default:
				break;
		}

		// Promotion
		if (move.get_promotion() != UNDEF_PIECE_TYPE) {
			move.get_ptr_piece()->set_type(move.get_promotion());
		}

		// En passant
		Square ep_tmp = board.get_en_passant();
		board.set_en_passant(move.get_en_passant());
		move.set_en_passant(ep_tmp);

		// 50 repetitions
		move.set_repetitions(board.get_repetitions()); // Save the current counter
		if (is_repetition) {
			board.inc_repetitions();
		}
		else {
			board.reset_repetitions();
		}

		// History
		board.positions_history.push_front(board.zobrist.get_key());
		board.ply++;
	}
}

void unmake_move(Board& board, Move& move) {
	
	// If it is not a null move
	if (move.get_type() != UNDEF_MOVE_TYPE) {

		board.change_turn_color();
		
		//cout << "DEBUG: Zobrist before unmaking " << move << " = \t" << board.zobrist << endl;
		board.zobrist.sub_move(move);
		//cout << "DEBUG: Zobrist after unmaking " << move << " = \t" << board.zobrist << endl;
	
		move.get_ptr_piece()->dec_nb_moves();
		board.set_ptr_piece(move.get_ptr_piece(), move.get_from());
		switch (move.get_type()) {
			case MOVE:
				break;
			case CAPTURE:
				board.set_ptr_piece(move.get_ptr_captured_piece(), move.get_to());
				break;
			case EN_PASSANT:
				//cout << "before unmake: " << move;
				//board.print();
				Square captured_from, tmp;
				tmp = (move.get_ptr_piece()->get_color() == WHITE) ? Square(move.get_to() + DOWN_LEFT) : Square(move.get_to() + UP_LEFT);
				captured_from = (move.get_from() == tmp) ? Square(move.get_from() + RIGHT) : Square(move.get_from() + LEFT);			
				board.set_ptr_piece(move.get_ptr_captured_piece(), captured_from);
				//cout << "after unmake: " << move;
				//board.print();
				break;
			case CASTLE:
				Square s;
				switch (move.get_to()) {
					case G1: s = H1; break;
					case C1: s = A1; break;
					case G8: s = H8; break;
					case C8: s = A8; break;
					default: s = OUT; break;
				}		
				//move.get_ptr_captured_piece()->set_position(s);
				board.set_ptr_piece(move.get_ptr_captured_piece(), s);
				move.get_ptr_captured_piece()->dec_nb_moves();
				Color color;
				color = (move.get_ptr_piece()->get_color() == WHITE) ? WHITE : BLACK;
				board.set_castling_right(color, true);
				break;
			default:
				break;
		}

		// Promotion
		if (move.get_promotion() != UNDEF_PIECE_TYPE) {
			move.get_ptr_piece()->set_type(PAWN);
		}
	
		// En passant
		Square ep_tmp = move.get_en_passant();
		move.set_en_passant(board.get_en_passant());
		board.set_en_passant(ep_tmp);
	
		// 50 repetitions
		board.set_repetitions(move.get_repetitions());

		// History
		board.positions_history.pop_front();
		board.ply--;
	}
}
