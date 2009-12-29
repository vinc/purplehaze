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

#include <iomanip>
//#include <iostream>
//#include <fstream>

#include "piece.h"
#include "move.h"
#include "board.h"
#include "movegen.h"
#include "attack.h"
#include "bench.h"
#include "init.h"

using namespace std;

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
	cout << i << "/" << n << "\t";
	cout << score << "\t";
	cout << (ply + 2) / 2 << (ptr_player->get_color() == BLACK ? ". .. " : ". ") << *ptr_best_move << "\t";
	cout << calculated_nodes << " nodes";
	cout << " (" << int((calculated_nodes / 1000) / elapsed_time) << "Knps)" << endl;
}

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

void print_perft(Pieces* ptr_player, Pieces* ptr_opponent) {
	// Put the pieces on the board and initialize it
	init_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");


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