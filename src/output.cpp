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
#include <iomanip>
#include <string>
#include <sstream>
#include <bitset>

#include "game.h"

using namespace std;

#define WIDE 10

void Game::print_thinking_header() {
    if (!output_thinking) return;
    cout << setw(4) << "ply";
    cout << setw(WIDE) << "score";
    cout << setw(WIDE) << "time";
    cout << setw(WIDE) << "nodes";
    cout << setw(WIDE) << "pv";
    cout << endl;
}

void Game::print_thinking(int depth, int score, Move m) {
    if (!output_thinking) return;
    cout << setw(4) << depth;
    cout << setw(WIDE) << score;
    cout << setw(WIDE) << time.get_elapsed_time();
    cout << setw(WIDE) << nodes_count;
    cout << setw(WIDE - 3) << " ";
    int ply = current_node().get_ply();
    if (ply % 2 != 0) cout << 1 + (ply / 2) << ". ...";
    //cout << m << endl; 
    assert(is_legal(m) || assert_msg(debug_move(m)));


    cout << output_pv(depth, score, m) << endl;
}

bool is_mate(int score) {
    if ((score < -INF - 100) || (INF - 100 < score)) return true;
    return false;
}

string Game::output_pv(int depth, int score, Move m) {
    ostringstream stream;
    stream << " ";
    int ply = current_node().get_ply();
    if (ply % 2 == 0) stream << 1 + (ply / 2) << ". ";
    stream << output_move(m);
    
    make_move(m);
    
    bool is_in_check = is_check(current_node().get_turn_color());
        
    // Find next move in TT
    Transposition trans = tt.lookup(current_node().hash());
    /*
    cout << endl;
    cout << "value=" << trans.get_value();
    cout << ", bound=" << trans.get_bound();
    cout << ", depth=" << trans.get_depth();
    cout << ", move=" << trans.get_best_move() << endl;
    */
    Move move = trans.get_best_move();
    if (depth > 0 && is_legal(move) && trans.get_bound() < 3) {
	if (is_in_check) stream << "+"; // Check
	stream << output_pv(depth - 1, trans.get_value(), move);
    }
    else if (move.is_null() && is_mate(score)) {
	if (is_in_check) stream << "#"; // Mate
    }
    else if (is_in_check) stream << "+"; // Cut-off
    
    undo_move(m);
    return stream.str();
}

string Game::output_move(Move m) {
    ostringstream stream;

    // Castling
    if (m.is_castle()) {
	if (m.get_castle_side() == QUEEN) stream << "O-";
	return stream.str() + "O-O";
    }

    // Type of piece
    Square from = m.get_orig();
    Piece p = board.get_piece(from);
    PieceType t = p.get_type();
    if (t > PAWN) stream << Piece(WHITE, t); // Upper case
    
    // Capture
    if (m.is_capture()) {
	if (t == PAWN) stream << char('a' + m.get_orig_file());
	else { // Disambiguation
	    Color c = Color(!current_node().get_turn_color());
	    Square to = m.get_dest();
	    for (int i = 0; i < pieces.get_nb_pieces(c, t); ++i) {
		Square s = pieces.get_position(c, t, i);
		if (s != from && board.can_attack(t, s, to)) {
		    stream << char('a' + m.get_orig_file());
		    break;
		}
	    }
	}
	stream << "x";
    }
    
    // Destination
    stream << output_square(m.get_dest_file(), m.get_dest_rank());
    
    // Promotion
    if (m.is_promotion()) {
	stream << "=" << Piece(WHITE, m.get_promotion_type());
    }

    return stream.str();
}

string Game::output_square(File f, Rank r) {
    ostringstream stream;
    stream << char('a' + f) << char('1' + r);
    return stream.str();
}

void Game::print_tt_stats() {
    long zeros = 0;
    long ones = 0;
    for (int i = 0; i < tt.size(); ++i) {
	if (tt.at(i).is_empty()) continue;
	Hash h = tt.at(i).get_hash();
	bitset<64> b = h;
	int z = b.count();
	//cout << "0: " << 64 - z << ", 1: " << z << endl;
	zeros += 64 - z;
	ones += z;
    }

    cout << "Zobrist:          " << hex << current_node().hash() << dec << endl;
    cout << "TT Size:          " << TT_SIZE / 1024 / 1024 << "Mb" << endl;
    cout << "Entries:          " << tt.size() << endl;
    cout << "Usage:            " << tt.get_usage();
    float percent_usage = (100 * tt.get_usage()) / float(tt.size());
    cout << " (" << percent_usage << "%)" << endl;
    float percent_zeros = (100.0 * zeros) / (64.0 * tt.get_usage());
    cout << "0's:              " << percent_zeros << "%" << endl;
    float percent_ones = (100.0 * ones) / (64.0 * tt.get_usage());
    cout << "1's:              " << percent_ones << "%" << endl;
    
    cout << "Lookups:          " << tt.get_nb_lookups() << endl;
    
    cout << "Hits:             " << tt.get_nb_hits();
    float percent_hits = (100 * tt.get_nb_hits()) / 
			 float(tt.get_nb_lookups());
    cout << " (" << percent_hits << "%)" << endl;
    
    cout << "Index Collisions: " << tt.get_nb_collisions();
    float percent_collisions = (100 * tt.get_nb_collisions()) / 
			       float(tt.get_nb_lookups());
    cout << " (" << percent_collisions << "%)" << endl;
    
    cout << "Misses:           " << tt.get_nb_misses();
    float percent_misses = (100 * tt.get_nb_misses()) / 
			   float(tt.get_nb_lookups());
    cout << " (" << percent_misses << "%)" << endl;
  
    cout << endl << "Material HashTable" << endl;
    HashTable<int>& table = material_table;
    cout << "MT Size:          " << MT_SIZE / 1024 / 1024 << "Mb" << endl;
    cout << "Entries:          " << table.size() << endl;
    cout << "Usage:            " << table.get_usage();
    percent_usage = (100 * table.get_usage()) / float(table.size());
    cout << " (" << percent_usage << "%)" << endl;
    percent_zeros = (100.0 * zeros) / (64.0 * table.get_usage());
    cout << "0's:              " << percent_zeros << "%" << endl;
    percent_ones = (100.0 * ones) / (64.0 * table.get_usage());
    cout << "1's:              " << percent_ones << "%" << endl;
    
    cout << "Lookups:          " << table.get_nb_lookups() << endl;
    
    cout << "Hits:             " << table.get_nb_hits();
    percent_hits = (100 * table.get_nb_hits()) / 
			 float(table.get_nb_lookups());
    cout << " (" << percent_hits << "%)" << endl;
    
    cout << "Index Collisions: " << table.get_nb_collisions();
    percent_collisions = (100 * table.get_nb_collisions()) / 
			       float(table.get_nb_lookups());
    cout << " (" << percent_collisions << "%)" << endl;
    
    cout << "Misses:           " << table.get_nb_misses();
    percent_misses = (100 * table.get_nb_misses()) / 
			   float(table.get_nb_lookups());
    cout << " (" << percent_misses << "%)" << endl;
}

string Game::debug_move(Move m) {
    ostringstream stream;
    Color c = current_node().get_turn_color();
    stream << endl << board << endl <<
	      (c == WHITE ? "White" : "Black") << " to move" << endl <<
	      "m = " << output_move(m) << " (" << m << ")" << endl <<
	      "m is en passant: " << m.is_en_passant() << endl <<
	      "m is promotion: " << m.is_promotion() << endl <<
	      "m is legal: " << is_legal(m) << endl <<
	      hex << current_node().hash();
    return stream.str();
}
