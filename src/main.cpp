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
#include "xboard.h"
#include "bench.h"
#include "transposition.h"

using namespace std;

Board board;
Pieces white_pieces(WHITE);
Pieces black_pieces(BLACK);

//Transpositions tt;


int main() {
	cout << "PurpleHaze 1.0 Copyright (C) 2009 Vincent Ollivier" << endl;
	cout <<	"This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'." << endl;
	cout <<	"This is free software, and you are welcome to redistribute it" << endl;
	cout <<	"under certain conditions; type 'show c' for details." << endl;
	
	cout << endl;

	/*
	cout << "Size of Square: " << sizeof(Square) << endl;
	cout << "Size of PieceType: " << sizeof(PieceType) << endl;
	cout << "Size of MoveType: " << sizeof(MoveType) << endl;
	cout << "Size of Move: " << sizeof(Move) << endl;
	cout << "Size of Piece: " << sizeof(Piece) << endl;
	cout << "Size of Board: " << sizeof(Board) << endl;
	cout << "Size of Hash: " << sizeof(Hash) << endl;
	cout << "Size of Zobrist: " << sizeof(Zobrist) << endl;
	cout << "Size of Transposition: " << sizeof(Transposition) << endl;
	//*/

	cout << endl;
	cout << "Compiled with the following options: ";
	#ifdef OPENING_BOOK
	cout << "OB, ";
	#endif
	#ifdef CHECK_EXTENSION
	cout << "CE, ";
	#endif
	#ifdef QUIESCENCE_SEARCH
	cout << "QS, ";
	#endif
	#ifdef DELTA_PRUNING
	cout << "DP, ";
	#endif
	#ifdef MVV_LVA
	cout << "MVV-LVA, ";
	#endif
	#ifdef KILLER_HEURISTIC
	cout << "KE, ";
	#endif
	#ifdef TRANSPOSITIONS_TABLE
	cout << "TT(CO=" << TT_STORE_CUTOFF << "), ";
	#endif
	#ifdef NULL_MOVE_PRUNING
	cout << "NMP(R=" << REDUCED_DEPTH << "), ";
	#endif
	#ifdef LATE_MOVE_REDUCTION
	cout << "LMR, ";
	#endif
	cout << "PVS, IT" << endl;

	
	srand(1);
	
	// Parse commands from CLI
	string cmd;
	cout << endl << "> ";
	cin >> cmd;
	while (cmd != "quit") {
		// Display help
		if (cmd == "help") {
			//cout << "bench" << endl;
			cout << "divide" << endl;
			cout << "help" << endl;
			cout << "setboard" << endl;
			cout << "perft" << endl;
			cout << "xboard" << endl;
			cout << "	new" << endl;
			cout << "	level MOVES TIME" << endl;
			cout << "	go" << endl;
			cout << "	MOVE" << endl;
			cout << "	move MOVE" << endl;
			cout << endl;
		}
		// Go to xboard mode
		else if (cmd == "xboard") {
			
			// Go to xboard mode
			xboard_loop();
			
			return 0;
		}

		/*
		else if (cmd == "setboard") {
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
			board.print();
		}
		//*/
		
		
		else if (cmd == "perft") {
			//Pieces* ptr_player = &white_pieces;
			//Pieces* ptr_opponent = &black_pieces;
			//print_perft(ptr_player, ptr_opponent);
			int depth = 0;
			cin >> depth;
			if (depth) {
				print_perft_fen(white_pieces, black_pieces, depth + 1);
			}
			else {
				print_perft(&white_pieces, &black_pieces);
			}
		}
		else if (cmd == "divide") {
			int depth = 0;
			cin >> depth;
			divide(white_pieces, black_pieces, depth);

		}
		

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
