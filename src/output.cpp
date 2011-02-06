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
    assert(is_legal(m) || assert_msg(
	    endl << board << endl << 
	    "m = " << output_move(m) << " (" << m << ")" << endl <<
	    "m is en passant: " << m.is_en_passant()
	));

    cout << output_principal_variation(depth, m) << endl;
}

string Game::output_principal_variation(int depth, Move m) {
    ostringstream stream;
    stream << " ";
    int ply = current_node().get_ply();
    if (ply % 2 == 0) stream << 1 + (ply / 2) << ". ";
    stream << output_move(m);
    Node pos = current_node();
    make_move(m);
    if (is_check(current_node().get_turn_color())) stream << "+";
    
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
	stream << output_principal_variation(depth - 1, move);
    }

    undo_move(m);
    assert(pos.hash() == current_node().hash());
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
	    Color c = current_node().get_turn_color(); // FIXME invert color
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
}
