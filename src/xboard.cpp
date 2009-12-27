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

#include <sstream>

#include <string> 
#include <iomanip>
#include <iostream>
#include <cstdio>
#include <ctime>

#include "piece.h"
#include "move.h"
#include "zobrist.h"
#include "board.h"
#include "movegen.h"
#include "search.h"
#include "eval.h"
#include "xboard.h"
#include "attack.h"
#include "transposition.h"
#include "book.h"
#include "init.h"

using namespace std;

//extern Transpositions tt;
Transpositions tt;

Book book;
int thinking_time = MAX_THINKING_TIME;

#ifdef DEBUG
bool root_search_debug = true;
#else
bool root_search_debug = false;
#endif

void print_principal_variation(Move& variation_move, int depth) {
	cout << " " << variation_move.get_san_notation();
	if (depth > 0) {
		make_move(board, variation_move);

		Piece* ptr_king_player = (variation_move.get_ptr_piece()->get_color() == WHITE) ? black_pieces.get_ptr_king() : white_pieces.get_ptr_king();
		Square s = ptr_king_player->get_position();
		Color c = (ptr_king_player->get_color() == WHITE) ? BLACK : WHITE;
		Pieces attackers = is_attacked_by(board, s, c);
		if (attackers.size() != 0) {
			cout << "+";
		}
	
		Transposition* ptr_trans = tt.lookup(board.zobrist.get_key());
		if (ptr_trans) {
			Move m = ptr_trans->get_best_move();
			print_principal_variation(m, depth - 1);
		}
		unmake_move(board, variation_move);
	}
	//cout << "-" << variation_move;
}

/** Find the best move to play with the given algo
  * on the given depth. */
Move* find_move_to_play(Pieces* ptr_player, Pieces* ptr_opponent, SearchAlgo algo, int depth) {
	// Debug informations
	int wide = 10;
	if (root_search_debug) {
		board.print();
		cout << setw(wide) << "depth";
		cout << setw(wide) << "time";
		cout << setw(wide) << "score";
		cout << setw(wide) << "moves";
		cout << endl;
	}

	// Initialize the search variables
	int alpha = -INF, beta = +INF, score = -INF;

	/*
	//TODO Check extension in the root search?
	if (is_in_check(board, ptr_player)) {
		++depth;
	}
	//*/

	std::clock_t start = std::clock();
	
	bool legal_move_found = false;
	Move* ptr_best_move = 0;
	Moves moves = movegen(board, *ptr_player);
	
	for (int ply = 1; ply < depth; ++ply) {
		
		//moves.sort();
		if (ptr_best_move) {
			moves.order(/*board,*/ ptr_best_move);
		}
		
		// No aspiration windows for now...
		alpha = -INF;
		beta = +INF;
		
		//cout << "Depth " << ply << endl;
		int i = 0;
		for (moves.iterator = moves.begin(); moves.iterator != moves.end(); ++moves.iterator) {
			++i;
			// Stop searching if end of thinking time
			if (((std::clock() - start) / CLOCKS_PER_SEC) > thinking_time)  {
				break;
			}

			Move* ptr_move = moves.get_ptr_move();
			
			make_move(board, *ptr_move);
		
			// Test if the move is legal
			bool is_legal_move = !is_in_check(board, ptr_player);
			if (is_legal_move) {
				legal_move_found = true;
			
				// Test if we have captured the king
				if (ptr_move->get_ptr_captured_piece() && ptr_move->get_ptr_captured_piece()->get_type() == KING) {
					// We won
					//unmake_move(board, *ptr_move); // We have to play the move chosed in root
					return ptr_best_move;
				}

				// Compute the score of this move
				switch (algo) {
					case NEGAMAX: 
						score = -negamax_search(board, *ptr_opponent, *ptr_player, ply); 
						break;
					case PVS: 
						score = -principal_variation_search(board, *ptr_opponent, *ptr_player, -beta, -alpha, ply, true); 
						break;
					default: 
						score = -alphabeta_search(board, *ptr_opponent, *ptr_player, -beta, -alpha, ply); 
						break;
				}
			}
			
			//Hash key = board.zobrist.get_key();
		
			unmake_move(board, *ptr_move);

			// If we have got a new best move, save it
			if (is_legal_move && score > alpha) {
				ptr_best_move = ptr_move;

				#ifdef TRANSPOSITIONS_TABLE	
				tt.save(board.zobrist.get_key(), score, alpha, beta, depth, *ptr_best_move);
				#endif
				
				alpha = score;

				if (root_search_debug) {
					cout << setw(wide) << ply;
					cout << setw(wide) << setprecision(3) << double(clock() - start)/CLOCKS_PER_SEC;;
					cout << setw(wide) << score;
				
					string str_ply;
					stringstream formatter;
					formatter << i << "/" << moves.size();
					str_ply.append(formatter.str());
					cout << setw(wide) << str_ply << " ";
					Move m = *ptr_move;
					print_principal_variation(m, ply);
					
					int mating_value = INF - 100;
					if (abs(score) > mating_value) {
						cout << " M" << ply + mating_value - abs(score);
					}
					cout << endl;
				}
			}
		}
	}
	
	if (legal_move_found && ptr_best_move) {
		#ifdef TRANSPOSITIONS_TABLE	
		tt.save(board.zobrist.get_key(), score, alpha, beta, depth, *ptr_best_move);
		#endif
	}
	return ptr_best_move;
}

/** Parse the given move and play it on the board.
  * Return true if the move have been played */
bool parse_and_play_move(string cmd) {		
	int f = (int(cmd[1]) - 49) * 16 + int(cmd[0]) - 97;
	int t = (int(cmd[3]) - 49) * 16 + int(cmd[2]) - 97;
	Square from = Square(f);
	Square to = Square(t);
	Piece* ptr_piece = board.get_ptr_piece(from);
	Piece* ptr_capture = board.get_ptr_piece(to);

	if (!ptr_piece) return false;
	
	// Castling
	if ((from == E1 && to == G1) ||
		(from == E1 && to == C1) ||
		(from == E8 && to == G8) ||
		(from == E8 && to == C8)
		) { 
		Piece* ptr_rook;
		Square s;
		switch (to) {
			case G1: s = H1; break;
			case C1: s = A1; break;
			case G8: s = H8; break;
			case C8: s = A8; break;
			default: s = OUT; break;
		}	
		ptr_rook = board.get_ptr_piece(s);
		if (ptr_rook) {
			Move m(ptr_piece, from, to, ptr_rook, CASTLE);
			make_move(board, m);
		}
	}

	// Promotion
	else if (cmd.size() == 5) {
		if (ptr_capture) {
			switch (cmd[4]) {
				case 'n':
					make_move(board, *new Move(ptr_piece, from, to, ptr_capture, KNIGHT));
					break;
				case 'b':
					make_move(board, *new Move(ptr_piece, from, to, ptr_capture, BISHOP));
					break;
				case 'r':
					make_move(board, *new Move(ptr_piece, from, to, ptr_capture, ROOK));
					break;
				case 'q':
					make_move(board, *new Move(ptr_piece, from, to, ptr_capture, QUEEN));
					break;
			}
		}
		else {
			switch (cmd[4]) {
				case 'n':
					make_move(board, *new Move(ptr_piece, from, to, KNIGHT));
					break;
				case 'b':
					make_move(board, *new Move(ptr_piece, from, to, BISHOP));
					break;
				case 'r':
					make_move(board, *new Move(ptr_piece, from, to, ROOK));
					break;
				case 'q':
					make_move(board, *new Move(ptr_piece, from, to, QUEEN));
					break;
			}
		}
	}

	// Other move
	else {	
		// Capture
		if (ptr_capture) {
			Move m(ptr_piece, from, to, ptr_capture);
			make_move(board, m);
		}
		// En passant
		else if (ptr_piece->get_type() == PAWN && to == board.get_en_passant()) {
			Square position;
			position = (ptr_piece->get_color() == WHITE) ? Square(to + DOWN) : Square(to + UP);
			Piece* ptr_capture = board.get_ptr_piece(position);
			if (ptr_capture
				&& ptr_capture->get_type() == PAWN
				&& ptr_capture->get_color() != ptr_piece->get_color()
				&& ptr_capture->get_nb_moves() == 1) {
				
				Move m(ptr_piece, from, to, ptr_capture);
				make_move(board, m);
			}
		}
		// Standard move
		else { 
			Move m(ptr_piece, from, to);
			make_move(board, m);
		}
	}
	return true;
}

/** Initialize the board and the engine,
  * parse commands from xboard,
  * print commands to xboard. */
void xboard_loop() {
	SearchAlgo algo = PVS;
	int depth = MAX_DEPTH;

	// By default engine play black
	Pieces* ptr_engine = &black_pieces;
	Pieces* ptr_xboard = &white_pieces;
	Color engine_color = BLACK; //TODO: this should temporary fixe the color bug
	
	string cmd;
	bool new_game = false;
	int nb_moves = 0, nb_time = 0;

	// Print a newline to acknowledge xboard mode
	cout << endl;

	// Parse command from xboard
	cin >> cmd;
	while (cmd != "quit") {
		//TODO: ptr_engine lose his color in the end of the game... Why?

		
		if (cmd == "new") { // Begin a new game
			new_game = true;
			// Put the pieces on the board and initialize it
			init_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		}
		else if (cmd == "level") { // Define the game level
			cin >> nb_moves; // Number of moves
			cin >> nb_time; // By lenght of time
			thinking_time = (nb_time * 60) / nb_moves; 
		}
		else if (cmd == "print") { // Print the board
			board.print();
		}
		else if (new_game && cmd == "go") { // Begin the game
			// Engine is asked to play white
			ptr_engine = &white_pieces;
			ptr_xboard = &black_pieces;
			engine_color = WHITE;

			#ifdef OPENING_BOOK
			cmd = book.get_move();
			if (!cmd.empty()) {
				parse_and_play_move(cmd);
				book.add_move(cmd);
				cout << "move " << cmd << endl;
			}
			else {			
			#endif
				// Search for a move to play
				Move* ptr_engine_move;
				ptr_engine_move = find_move_to_play(ptr_engine, ptr_xboard, algo, depth);

				if (ptr_engine_move) { // If we have find one
					make_move(board, *ptr_engine_move); // We play it
					cout << "move " << *ptr_engine_move << endl; // And we display it
				}
			#ifdef OPENING_BOOK
			}
			#endif
			
		}
		else if ( // Parse a move
			cmd.size() >= 4 &&
			cmd.size() <= 5 &&
			int(cmd[0]) >= 97 && int(cmd[0]) <= 104 &&
			int(cmd[1]) >= 49 && int(cmd[1]) <=  56 &&
			int(cmd[2]) >= 97 && int(cmd[2]) <= 104 &&
			int(cmd[3]) >= 49 && int(cmd[3]) <=  56
			) {
							
			// Xboard's move
			if (parse_and_play_move(cmd)) {			
				
				
				#ifdef OPENING_BOOK
				// Add the move played by xboard to the current line
				book.add_move(cmd);

				// Search in the book for an answer to it
				cmd = book.get_move();
				if (!cmd.empty()) {
					parse_and_play_move(cmd);
					book.add_move(cmd);
					cout << "move " << cmd << endl;
				}
				
				// If we do not find one, the we have to compute it
				else {	
				#endif
					// Search for a move to play
					Move* ptr_engine_move;
					ptr_engine_move = find_move_to_play(ptr_engine, ptr_xboard, algo, depth);

					if (ptr_engine_move) { // If we have find one
						make_move(board, *ptr_engine_move); // We play it
						cout << "move " << *ptr_engine_move << endl; // And we display it
					}
					else { // If we do not have a move to play
						if (is_in_check(board, ptr_engine)) { // And if we are in check
							// This is a Checkmate
							cout << ((engine_color == WHITE) ? "0-1 {White lose}" : "1-0 {Black lose}") << endl;
						}
						else { // If we are not in check
							// This is a Stalemate
							cout << "1/2-1/2 {Stalemate}" << endl;
						}
					}
				#ifdef OPENING_BOOK
				}
				#endif
			}
			else {
				cout << "Illegal move: " << cmd;
			}
		}
		cin >> cmd;
	}
}
