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

//#include <stdio.h>
//#include <time.h>
//#include <iomanip>
//#include <iostream>
//#include <fstream>
//#include <string>

#include "piece.h"
#include "board.h"
#include "init.h"
#include "xboard.h"


using namespace std;

Board board;
Pieces white_pieces(WHITE);
Pieces black_pieces(BLACK);

//Transpositions tt;

int perft_moves_counter = 0;
int perft_captures_counter = 0;
int perft_en_passant_counter = 0;
int perft_castles_counter = 0;
int perft_promotions_counter = 0;
int perft_checks_counter = 0;
int perft_checkmates_counter = 0;

Moves divide_moves;

void display_search(int score, int nodes, int time, int i, int n, int ply, Pieces* ptr_player, Move* ptr_best_move) {
	float elapsed_time = float(clock() - time)/CLOCKS_PER_SEC;
	int calculated_nodes = perft_moves_counter - nodes;
	cout << "\t" << i << "/" << n << "\t";
	cout << score << "\t";
	cout << (ply + 2) / 2 << (ptr_player->get_color() == BLACK ? ". .. " : ". ") << *ptr_best_move << "\t";
	cout << calculated_nodes << " nodes";
	cout << " (" << int((calculated_nodes / 1000) / elapsed_time) << "Knps)" << endl;
}

/*
// Should leave main.cpp
int perft(Pieces* ptr_player, Pieces* ptr_opponent, int depth) {
	int ret = 0;
	bool is_illegal = false;
		
	if (!is_illegal && depth > 0) {
		Moves moves = movegen(board, *ptr_player);
		
		if (moves.size() == 0) {
			++perft_checkmates_counter;
		}
		
		bool legal_move_exist = false;
		for (moves.iterator = moves.begin(); moves.iterator != moves.end(); ++moves.iterator) {
			Move* ptr_move = moves.get_ptr_move();
			make_move(board, *ptr_move);
			
			// Check detection
			Piece* ptr_king_player = ptr_player->get_ptr_king();
			Square s = ptr_king_player->get_position();
			Color c = (ptr_king_player->get_color() == WHITE) ? BLACK : WHITE;
			Pieces attackers = is_attacked_by(board, s, c);
			
			// If the position is legal we can going deeper
			if (attackers.size() == 0) {
				bool is_checkmate = false;
				legal_move_exist = true;
				++perft_moves_counter;
				switch (ptr_move->get_type()) {
					case CASTLE: 
						++perft_castles_counter; 
						break;
					case EN_PASSANT:
						++perft_en_passant_counter;
						++perft_captures_counter; 
						break;
					case CAPTURE: 
						++perft_captures_counter; 
						break;
					default: break;
				}
				
				if (!is_checkmate) {
					
					// Promotion
					if (ptr_move->get_promotion() != UNDEF_PIECE_TYPE) {
						++perft_promotions_counter;
					}

					// Add current node
					++ret;
					
					// Add childs nodes
					ret += perft(ptr_opponent, ptr_player, depth-1);
				}
				
				// Check if we put the other king in check
				Piece* ptr_king_opponent = ptr_opponent->get_ptr_king();
				s = ptr_king_opponent->get_position();
				c = (ptr_king_opponent->get_color() == WHITE) ? BLACK : WHITE;
				attackers = is_attacked_by(board, s, c);
				if (attackers.size() != 0) {
					++perft_checks_counter;
				}
			}
			
			unmake_move(board, *ptr_move);
		}
		
		if (!legal_move_exist) {
			// Check detection
			Piece* ptr_king_player = ptr_player->get_ptr_king();
			Square s = ptr_king_player->get_position();
			Color c = (ptr_king_player->get_color() == WHITE) ? BLACK : WHITE;
			Pieces attackers = is_attacked_by(board, s, c);
			if (attackers.size() != 0) {
				// Checkmate
				++perft_checkmates_counter;
			}
			else {
				// Pat
			}
		}
		
		moves.clear();
	}
	return ret;
}
*/

/*
// Should leave main.cpp
void divide(Pieces* ptr_player, Pieces* ptr_opponent, int depth) {
	Moves moves = movegen(board, *ptr_player);
	int nb_nodes = 0;
	int nb_moves = 0;
	bool legal_move_exist = false;
	for (moves.iterator = moves.begin(); moves.iterator != moves.end(); ++moves.iterator) {
		Move* ptr_move = moves.get_ptr_move();
		make_move(board, *ptr_move);
		
		// Check detection
		Piece* ptr_king_player = ptr_player->get_ptr_king();
		Square s = ptr_king_player->get_position();
		Color c = (ptr_king_player->get_color() == WHITE) ? BLACK : WHITE;
		Pieces attackers = is_attacked_by(board, s, c);
		
		// If the position is legal we can going deeper
		if (attackers.size() == 0) {
			bool is_checkmate = false;
			legal_move_exist = true;
			++perft_moves_counter;
			switch (ptr_move->get_type()) {
				case CASTLE: 
					++perft_castles_counter; 
					break;
				case EN_PASSANT:
					++perft_en_passant_counter;
				case CAPTURE: 
					++perft_captures_counter; 
					break;
				default: break;
			}
			
			if (!is_checkmate) {
				
				// Promotion
				if (ptr_move->get_promotion() != UNDEF_PIECE_TYPE) {
					++perft_promotions_counter;
				}

				int ret = perft(ptr_opponent, ptr_player, depth-1);
				ret -= perft(ptr_opponent, ptr_player, depth-2);
				nb_nodes += ret;
				++nb_moves;
				cout << *ptr_move << " " << ret << endl;
			}
			
			// Check if we put the other king in check
			Piece* ptr_king_opponent = ptr_opponent->get_ptr_king();
			s = ptr_king_opponent->get_position();
			c = (ptr_king_opponent->get_color() == WHITE) ? BLACK : WHITE;
			attackers = is_attacked_by(board, s, c);
			if (attackers.size() != 0) {
				++perft_checks_counter;
			}
		}
		
		unmake_move(board, *ptr_move);
	}
	
	if (!legal_move_exist) {
		// Check detection
		Piece* ptr_king_player = ptr_player->get_ptr_king();
		Square s = ptr_king_player->get_position();
		Color c = (ptr_king_player->get_color() == WHITE) ? BLACK : WHITE;
		Pieces attackers = is_attacked_by(board, s, c);
		if (attackers.size() != 0) {
			// Checkmate
			++perft_checkmates_counter;
		}
		else {
			// Pat
		}
	}
	cout << "Nodes: " << nb_nodes << endl;
	cout << "Moves: " << nb_moves << endl;
	moves.clear();
}
*/

int main() {
	cout << "PurpleHaze 1.0 Copyright (C) 2009 Vincent Ollivier" << endl;
	cout <<	"This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'." << endl;
	cout <<	"This is free software, and you are welcome to redistribute it" << endl;
	cout <<	"under certain conditions; type 'show c' for details." << endl;
	
	string cmd;
	cout << endl << "> ";
	cin >> cmd;
	while (cmd != "quit") {
		if (cmd == "help") {
			cout << "bench" << endl;
			cout << "help" << endl;
			cout << endl;
			cout << "xboard" << endl;
			cout << "new" << endl;
			cout << "level MOVES TIME" << endl;
			cout << "go" << endl;
			cout << "MOVE" << endl;
			cout << "move MOVE" << endl;
			cout << endl;
			cout << "bench" << endl;
		}
		else if (cmd == "xboard") {
			// Starting position
			init_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

			// Purplehaze #1 (avoid f7)
			//init_board("8/8/5P2/8/6P1/1p1k4/3P2P1/K7 w - - 0 60");
			
			// Kaufman #1 (Nf6+)
			//init_board("1rbq1rk1/p1b1nppp/1p2p3/8/1B1pN3/P2B4/1P3PPP/2RQ1R1K w - - 0 1 ");

			// Kaufman #3 (avoid Rd1)
			//init_board("3r2k1/1p3ppp/2pq4/p1n5/P6P/1P6/1PB2QP1/1K2R3 w - - 0 1 ");

			// Kaufman #16 (b3b4)
			//init_board("8/4p3/p2p4/2pP4/2P1P3/1P4k1/1P1K4/8 w - - 0 1");
			
			// Kaufman #21 (c4b5)
			//init_board("8/2k5/4p3/1nb2p2/2K5/8/6B1/8 w - - 0 1");

			// Kaufman #22 (Ba7)
			//init_board("1B1b4/7K/1p6/1k6/8/8/8/8 w - - 0 1 ");
			
			//init_board("8/6p1/5p2/7K/4kP1P/8/8/8 w - - 0 1");
				
			// White win in 2 moves
			//init_board("8/1k6/5R2/6R1/8/8/3K4/8 w - - 0 1");

			// Black win in 2 moves
			//init_board("8/1K6/5r2/6r1/8/8/3k4/8 w - - 0 1");

			// Test SSE
			//init_board("3qk3/b7/4n3/8/3p4/5N2/1BN5/2KQ4 w - - 0 1");
			
			// M2
			//init_board("8/6P1/8/k1K5/8/8/8/8 w - - 0 1");
			
			// M3
			//init_board("8/4P1n1/8/R5P1/8/8/P1K5/k7 w - - 0 1");
			
			// Rh8!
			//init_board("R7/P4k2/8/8/8/8/r7/6K1 w - - 0 1");
			
			//board.print();			
			
			cout << endl;
			xboard_loop();
			return 0;
		}

		/*
		else if (cmd == "divide") {
			int depth;
			cin >> depth;
			Pieces* ptr_player = &white_pieces;
			Pieces* ptr_opponent = &black_pieces;
			divide(ptr_player, ptr_opponent, depth);
		}
		*/
		
		else if (cmd == "setboard") {
			//init_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

			///*
			string piece_placement, side_to_move, castling_ability, en_passant_target_square, reversible_halfmove_counter, fullmove_counter;
			string fen;
			
			cin >> piece_placement >> side_to_move >> castling_ability >> en_passant_target_square >> reversible_halfmove_counter >> fullmove_counter;
			
			fen.append(piece_placement);
			fen.append(" ");
			fen.append(side_to_move);
			fen.append(" ");
			fen.append(castling_ability);
			fen.append(" ");
			fen.append(en_passant_target_square);
			fen.append(" ");
			fen.append(reversible_halfmove_counter);
			fen.append(" ");
			fen.append(fullmove_counter);

			init_board(fen.c_str());
			/*
			//init_board("K1k5/8/8/8/5pP1/8/4P3/8 b - g3 0 1");
			board.set_en_passant(G3);
			board.get_ptr_piece(G4)->inc_nb_moves();
			//*/
			
			board.print();
		}
		/*
		else if (cmd == "perft") {
			Pieces* ptr_player = &white_pieces;
			Pieces* ptr_opponent = &black_pieces;
			int calculated_nodes = 0, nb_captures = 0, nb_castles = 0, nb_checks = 0, nb_checkmates = 0, nb_en_passant = 0, nb_promotions = 0;
			int total_nodes = 0, total_captures = 0, total_castles = 0, total_checks = 0, total_checkmates = 0, total_en_passant = 0, total_promotions = 0;			
			int wide = 13;
			cout << setw(wide) << "Depth";
			cout << setw(wide) << "Nodes";
			cout << setw(wide) << "Captures";
			cout << setw(wide) << "En passant";
			cout << setw(wide) << "Castles";
			cout << setw(wide) << "Promotion";
			cout << setw(wide) << "Checks";
			cout << setw(wide) << "Checkmates";
			cout << setw(wide) << "Time";
			cout << endl;
			for (int depth = 1; depth < 10; ++depth) {
				clock_t starting_time = clock();
				int nodes = perft_moves_counter;
				int captures = perft_captures_counter;
				int en_passant = perft_en_passant_counter;
				int castles = perft_castles_counter;
				int promotions = perft_promotions_counter;
				int checks = perft_checks_counter;
				int checkmates = perft_checkmates_counter;
				
				perft(ptr_player, ptr_opponent, depth);
				
				float elapsed_time = float(clock() - starting_time)/CLOCKS_PER_SEC;
				calculated_nodes = perft_moves_counter - nodes - total_nodes;
				nb_captures = perft_captures_counter - captures - total_captures;
				nb_en_passant = perft_en_passant_counter - en_passant - total_en_passant;
				nb_castles = perft_castles_counter - castles - total_castles;
				nb_promotions = perft_promotions_counter - promotions - total_promotions;
				nb_checks = perft_checks_counter - checks - total_checks;
				nb_checkmates = perft_checkmates_counter - checkmates - total_checkmates;
				
				total_nodes += calculated_nodes;
				total_captures += nb_captures;
				total_en_passant += nb_en_passant;
				total_castles += nb_castles;
				total_promotions += nb_promotions;
				total_checks += nb_checks;
				total_checkmates += nb_checkmates;
				
	
				cout << setw(wide) << depth;
				cout << setw(wide) << calculated_nodes;
				cout << setw(wide) << nb_captures;
				cout << setw(wide) << nb_en_passant;
				cout << setw(wide) << nb_castles;
				cout << setw(wide) << nb_promotions;
				cout << setw(wide) << nb_checks;
				cout << setw(wide) << nb_checkmates;
				cout << setw(wide) << setprecision(4) << elapsed_time << endl;
	
			}	
		}
		*/

		/*
		else if (cmd == "test") {
			//init_board("1k3q2/8/5r2/4n2b/4p1p1/1r1R1K2/8/8 w - -");
			init_board("7n/3P2P1/5K2/8/8/8/1k1p4/4R3 w - -");
			board.print();
			
			Pieces attackers = is_attacked_by(board, F3, BLACK);
			for (attackers.iterator = attackers.begin(); attackers.iterator != attackers.end(); ++attackers.iterator) {
				Piece* ptr_piece = attackers.get_ptr_piece();
				cout << *ptr_piece << " is attacking F3" << endl;
			}
			
		}
		*/
		cout << endl << "> ";
		cin >> cmd;
	}	
	return 0;
}
