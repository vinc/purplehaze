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
#include "search.h"
#include "eval.h"
#include "attack.h"
#include "transposition.h"

using namespace std;

extern Transpositions tt;

/****************************************************************
 *                                                              *
 *                       Quiescence Search                      *
 *                                                              *
 ****************************************************************/

int quiescence_search(Board& board, Pieces& player, Pieces& opponent, int alpha, int beta) {
	int score, stand_pat = eval(board, player, opponent);
	if (stand_pat >= beta) {
		return beta;
	}
	if (alpha < stand_pat) {
		alpha = stand_pat;
	}
	
	//TODO: replace with a captures only generator
	Moves moves = movegen(board, player, true);
	
	//moves.order(/*board,*/ 0);
	
	for (moves.iterator = moves.begin(); moves.iterator != moves.end(); ++moves.iterator) {
		Move* ptr_move = moves.get_ptr_move();
		
		if (ptr_move->get_type() == CAPTURE){
			//TODO add en passant in the condition?
			
			#ifdef SEE
			Square s = ptr_move->get_to();
			Color c = ptr_move->get_ptr_piece()->get_color();

			bool debug = false;
			//cout << "QS: " << *ptr_move;
			if (debug) cout << endl << "Requesting SEE for square " << static_cast<char>(97 + (s & 7)) << 1 + (s >> 4);
			if (debug) cout << "(" << *ptr_move << ", " << *ptr_move->get_ptr_piece() << ", " << *ptr_move->get_ptr_captured_piece() << ")" << endl;	
			if (debug) cout << "Board before QS capture:" << endl;
			if (debug) board.print();
			#endif
			
			make_move(board, *ptr_move);
				
			
			#ifdef SEE
			//cout << "Zobrist key before SEE " << board.zobrist.get_key() << endl;
			if (debug) cout << "Board after QS capture:" << endl;
			if (debug) board.print();
			int see = -static_exchange_evaluation(board, s, c);
			//cout << "Zobrist key after SEE " << board.zobrist.get_key() << endl;	
			//cout << "SEE: " << see << endl;
			if (see > 0) {
				if (debug) cout << "The move does not worth it!" << endl;
				unmake_move(board, *ptr_move);
				continue;
			}
			#endif
				
			score = -quiescence_search(board, opponent, player, -beta, -alpha);
		
			if (score >= beta) {
				unmake_move(board, *ptr_move);
				return beta;
			}
			if (score > alpha) {
				alpha = score;
			}
				
			unmake_move(board, *ptr_move);
		}
    }
    return alpha;
}

/****************************************************************
 *                                                              *
 *                            Negamax                           *
 *                                                              *
 ****************************************************************/

int negamax_search(Board& board, Pieces& player, Pieces& opponent, int depth) {
	int is_in_check;
	Piece* ptr_king_player = player.get_ptr_king();
	Square s = ptr_king_player->get_position();
	Color c = (ptr_king_player->get_color() == WHITE) ? BLACK : WHITE;
	Pieces attackers = is_attacked_by(board, s, c);
	is_in_check = (attackers.size() == 0) ? false : true;
	
	// Check extension
	/*
	if (depth == 0 && is_in_check && depth < MAX_DEPTH) {
		++depth;
	}
	*/
	
	
	if (depth == 0) {
		return eval(board, player, opponent);
	}

	// 50 repetitions
	if (board.get_repetitions() >= 100) {
		return 0;
	}		


	int score, max = -INF; // Do we initialize it to -INF or leave it w/o being init?
	bool legal_move_found = false;
	Moves moves = movegen(board, player, false);
	for (moves.iterator = moves.begin(); moves.iterator != moves.end(); ++moves.iterator) {
		Move* ptr_move = moves.get_ptr_move();
		
		make_move(board, *ptr_move);
		
		// Test if the move is legal
		s = ptr_king_player->get_position();
		attackers = is_attacked_by(board, s, c);
		if (attackers.size() == 0) {
			legal_move_found = true;
		
			// Test if we have captured the king
			if (ptr_move->get_ptr_captured_piece() && ptr_move->get_ptr_captured_piece()->get_type() == KING) {
				// We won
				moves.clear();
				unmake_move(board, *ptr_move);
				//cout << "capture at " << depth << " by " << *ptr_move << endl;
				return INF - 100 + depth;
			}
			
			//TODO we could put alphabeta() here
			// but what if score is not inisialised?
		}
		
		score = -negamax_search(board, opponent, player, depth - 1);
		
		unmake_move(board, *ptr_move);
		
		if (score > max) {
			max = score;
		}
	}
	moves.clear();
	
	// If there is no legal moves
	if (!legal_move_found) {
		// Checkmate
		if (is_in_check) {
			return -INF + 100 - depth;
		}
		// Stalemate
		else {
			return 0;
		}
	}
	
	return max;
}

/****************************************************************
 *                                                              *
 *                          Alpha Beta                          *
 *                                                              *
 ****************************************************************/

int alphabeta_search(Board& board, Pieces& player, Pieces& opponent, int alpha, int beta, int depth) {
	int is_in_check;
	Piece* ptr_king_player = player.get_ptr_king();
	Square s = ptr_king_player->get_position();
	Color c = (ptr_king_player->get_color() == WHITE) ? BLACK : WHITE;
	Pieces attackers = is_attacked_by(board, s, c);
	is_in_check = (attackers.size() == 0) ? false : true;
	
	// Check extension
	/*
	if (depth == 0 && is_in_check && depth < MAX_DEPTH) {
		++depth;
	}
	*/
	
	
	if (depth == 0) {
		#ifdef QUIESCENCE_SEARCH
		return quiescence_search(board, player, opponent, alpha, beta);
		#else
		return eval(board, player, opponent);
		#endif
	}

	// 50 repetitions
	if (board.get_repetitions() >= 100) {
		return 0;
	}

	
	Move* ptr_best_move = 0;
	
	#ifdef TRANSPOSITIONS_TABLE
	// Look in the transposition table before searching
	Transposition* ptr_trans = tt.lookup(board.zobrist.get_key());
	if (ptr_trans && ptr_trans->get_depth() >= depth) {
		//cout << "Entry in TT found!" << endl;
		switch (ptr_trans->get_bound()) {
			case LOWER:
				alpha = (alpha > ptr_trans->get_value() ? alpha : ptr_trans->get_value());
				break;
			case UPPER:
				beta = (beta < ptr_trans->get_value() ? beta : ptr_trans->get_value());
				break;
			case ACCURATE:
				alpha = ptr_trans->get_value();
				beta = alpha;
				break;
			default: break;
		}
		if (alpha >= beta) {
			// tt cause a cutoff
			//cout << "Cutoff!" << endl;
			return ptr_trans->get_value();
		}
		//ptr_best_move = &ptr_trans->get_best_move();
	}
	#endif

	int score; // Do we initialize it to -INF or leave it w/o being init?
	bool legal_move_found = false;
	
	Moves moves = movegen(board, player, false);
	/*
	if (ptr_best_move) {
		moves.find(*ptr_best_move)
	}
	 */
	
	for (moves.iterator = moves.begin(); moves.iterator != moves.end(); ++moves.iterator) {
		Move* ptr_move = moves.get_ptr_move();
		
		make_move(board, *ptr_move);
		
		// Test if the move is legal
		s = ptr_king_player->get_position();
		attackers = is_attacked_by(board, s, c);
		if (attackers.size() == 0) {
			legal_move_found = true;
		
			// Test if we have captured the king
			if (ptr_move->get_ptr_captured_piece() && ptr_move->get_ptr_captured_piece()->get_type() == KING) {
				// We won
				moves.clear();
				unmake_move(board, *ptr_move);
				//cout << "capture at " << depth << " by " << *ptr_move << endl;
				return INF - 100 + depth;
			}
			
			//TODO we could put alphabeta() here
			// but what if score is not inisialised?
		}
		
		score = -alphabeta_search(board, opponent, player, -beta, -alpha, depth - 1);
		
		unmake_move(board, *ptr_move);
		
		if (score >= beta) {
			moves.clear();
			return beta;
		}
		if (score > alpha) {
			ptr_best_move = ptr_move;
			alpha = score;
		}
	}
	
	// If there is no legal moves
	if (!legal_move_found) {
		// Checkmate
		if (is_in_check) {
			moves.clear();
			return -INF + 100 - depth;
		}
		// Stalemate
		else {
			moves.clear();
			return 0;
		}
	}

	#ifdef TRANSPOSITIONS_TABLE
	// Save to transposition table
	int value = alpha; // ?
	tt.save(board.zobrist.get_key(), value, alpha, beta, depth, *ptr_best_move);
	#endif

	moves.clear();
	return alpha;
}


/****************************************************************
 *                                                              *
 *                Principal Variation Search                    *
 *                                                              *
 ****************************************************************/

int principal_variation_search(Board& board, Pieces& player, Pieces& opponent, int alpha, int beta, int depth, bool null_move_pruning) {
	int score = -INF, old_alpha = alpha; // Do we initialize it to -INF or leave it w/o being init?
	
	// 50 repetitions
	if (board.get_repetitions() >= 100) {
		return 0;
	}

	// Threefold repetition
	list<Hash>::iterator it = board.positions_history.begin();
	int nb_repetitions = 1;
	Hash current_position = *it;
	bool side_to_move = false;
	for (++it; it != board.positions_history.end(); ++it) {
		if (side_to_move && current_position == *it) ++nb_repetitions;
		if (nb_repetitions > 2) break;
		side_to_move = !side_to_move;
	}
	if (nb_repetitions > 2) {
		// Repetition detected!
		return 0;
	}
	
	Piece* ptr_king_player = player.get_ptr_king();
	Square s = ptr_king_player->get_position();
	Color c = (ptr_king_player->get_color() == WHITE) ? BLACK : WHITE;
	Pieces attackers = is_attacked_by(board, s, c);
	bool is_in_check = (attackers.size() == 0) ? false : true;
	bool is_still_in_check = false;
	
	// Check extension
	#ifdef CHECK_EXTENSION
	if (is_in_check && depth < MAX_DEPTH ) {
		++depth;
	}
	#endif
	
	// Quiescence Search
	if (depth <= 0) {
		#ifdef QUIESCENCE_SEARCH
		return quiescence_search(board, player, opponent, alpha, beta);
		#else
		return eval(board, player, opponent);
		#endif
	}

	Move* ptr_best_move = 0;

	#ifdef TRANSPOSITIONS_TABLE
	// Look in the transposition table before searching
	Transposition* ptr_trans = tt.lookup(board.zobrist.get_key());
	if (ptr_trans && ptr_trans->get_depth() >= depth) {
		//cout << "Entry in TT found!" << endl;
		switch (ptr_trans->get_bound()) {
			case LOWER:
				alpha = (alpha > ptr_trans->get_value() ? alpha : ptr_trans->get_value());
				//cout << "LOWER" << endl;
				break;
			case UPPER:
				beta = (beta < ptr_trans->get_value() ? beta : ptr_trans->get_value());
				//cout << "UPPER" << endl;
				break;
			case ACCURATE:
				alpha = ptr_trans->get_value();
				beta = alpha;
				//cout << "ACCURATE" << endl;
				break;
			default: break;
		}
		if (alpha >= beta) {
			// tt cause a cutoff
			//cout << "Cutoff!" << endl;
			return ptr_trans->get_value();
		}
		//ptr_best_move = &ptr_trans->get_best_move();
		Move best_move = ptr_trans->get_best_move();
		ptr_best_move = &best_move;
		
	}
	#endif
	
	// Null move pruning
	#ifdef NULL_MOVE_PRUNING
	if (!is_in_check && null_move_pruning && depth > 3) {
		//Move null_move(0, OUT, OUT, UNDEF_MOVE_TYPE);
		//make_move(board, null_move);
		// Null move search with minimal window around beta
		score = -principal_variation_search(board, opponent, player, -beta, -beta + 1, depth - REDUCED_DEPTH - 1, false);
		//unmake_move(board, null_move);
		if (score >= beta) { // Cutoff in case of fail-high
			return score;
		}
	}
	#endif
	
	bool is_principal_variation = true;
	int moves_searched = 0;
	
	bool legal_move_found = false;
	
	int best_score = -INF;
	Moves moves = movegen(board, player, false);

	moves.order(/*board,*/ ptr_best_move);	
	
	for (moves.iterator = moves.begin(); moves.iterator != moves.end(); ++moves.iterator) {
		Move* ptr_move = moves.get_ptr_move();
		
		make_move(board, *ptr_move);
		
		// Test if the move is legal
		s = ptr_king_player->get_position();
		attackers = is_attacked_by(board, s, c);
		is_still_in_check = (attackers.size() == 0) ? false : true;
		if (!is_still_in_check) {
			legal_move_found = true;
		
			// If we have captured the king, we won
			if (ptr_move->get_ptr_captured_piece() && ptr_move->get_ptr_captured_piece()->get_type() == KING) {
				unmake_move(board, *ptr_move);
				moves.clear();
				return INF - 100 + depth;
			}
			
			if (is_principal_variation) {
				score = -principal_variation_search(board, opponent, player, -beta, -alpha, depth - 1, null_move_pruning);
			}
			else {
				// Late move reduction
				#ifdef LATE_MOVE_REDUCTION
				s = opponent.get_ptr_king()->get_position();
				attackers = is_attacked_by(board, s, ptr_king_player->get_color());
				bool is_giving_check = (attackers.size() == 0) ? false : true;
			
				if (moves_searched >= 4 
					&& depth > 3
					&& !is_still_in_check
					&& !is_giving_check
					&& ptr_move->get_type() != CAPTURE
					&& ptr_move->get_type() != EN_PASSANT
					&& ptr_move->get_promotion() == UNDEF_PIECE_TYPE) {
					// Search this move with reduced depth
					score = -principal_variation_search(board, opponent, player, -alpha - 1, -alpha, depth - 2, null_move_pruning);
				}
				else {
				#endif
		
					score = -principal_variation_search(board, opponent, player, -alpha - 1, -alpha, depth - 1, null_move_pruning);
					if (score > alpha && score < beta) { // Re-search
						score = -principal_variation_search(board, opponent, player, -beta, -alpha, depth - 1, null_move_pruning); 
					}
			
				#ifdef LATE_MOVE_REDUCTION
				}
				#endif
			
			}
			if (score > alpha) {
				if (score >= beta) {
					unmake_move(board, *ptr_move);
					#ifdef TRANSPOSITIONS_TABLE			
					//TODO false the results
					//tt.save(board.zobrist.get_key(), score, alpha, beta, depth, *ptr_move);
					#endif
					//cout << "Fail-hard beta cutoff: alpha=" << alpha << " score=" << score << " beta=" << beta << endl;
					
					moves.clear();
					return beta; // Fail-hard beta cutoff
					//return score; // Fail-soft
				}
				
				old_alpha = alpha;
				alpha = score; // alpha acts like max in MiniMax
				best_score = score;
				ptr_best_move = ptr_move;			
			}
			
			is_principal_variation = false;
			++moves_searched;
		}
		unmake_move(board, *ptr_move);
	}
	
	// If there is no legal moves
	if (!legal_move_found) {
		// Checkmate
		if (is_in_check) {
			moves.clear();
			return -INF + 100 - depth;
		}
		// Stalemate
		else {
			moves.clear();
			return 0;
		}
	}

	#ifdef TRANSPOSITIONS_TABLE	
	if (ptr_best_move) {
		tt.save(board.zobrist.get_key(), best_score, old_alpha, beta, depth, *ptr_best_move);
		//cout << "End loop: alpha=" << alpha << " score=" << score << " beta=" << beta << " old_alpha=" << old_alpha << endl;
	}
	#endif
	
	moves.clear();
	return alpha; // fail-hard

}
