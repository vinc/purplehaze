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
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "game.h"
#include "eval.h"
#include "hashtable.h"

using namespace std;

// PST[Phase][Color][PieceType][Square]
static int PST[2][2][int(KING) + 1][BOARD_SIZE] = { { { { 0 } } } };

void Game::init_eval() {
    for (int i = 0; i < 64; ++i) {
        for (PieceType t = PAWN; t <= KING; t = PieceType(t + 1)) {
            Square s = board.get_square(i);
            
            int opening_score = 0;
            int ending_score = 0;

            switch (t) {
                case PAWN: 
                    // Develop central pawns
                    // But not side pawns
                    opening_score = PAWN_FILES_VALUES[board.get_file(s)];

                    // Run for promotion
                    ending_score = 10 * board.get_rank(s);
                    break;
                case KNIGHT:
                case BISHOP:
                    // Develop toward center files
                    opening_score = CENTER_BONUS[board.get_file(s)];
                    if (board.is_border(s)) {
                        opening_score = 2 * BORDER_MALUS;
                    }
                    // no break
                default:
                    ending_score = CENTER_BONUS[board.get_file(s)];
                    ending_score += CENTER_BONUS[board.get_rank(s)];
                    break;
            }

            // Rank bonus
            int bonus = OPENING_RANKS_BONUS[t][board.get_rank(s)];
            opening_score += (opening_score * bonus) / 2;

            PST[OPENING][WHITE][t][s] = opening_score;
            PST[ENDING][WHITE][t][s] = ending_score;
        }
    }
    // Special corrections
    // Urge to develop light pieces during oppening
    PST[OPENING][WHITE][KNIGHT][B1] = -20;
    PST[OPENING][WHITE][KNIGHT][G1] = -20;
    PST[OPENING][WHITE][BISHOP][C1] = -15;
    PST[OPENING][WHITE][BISHOP][F1] = -15;
    // But others should stay where they are
    PST[OPENING][WHITE][ROOK][A1]   = 5;
    PST[OPENING][WHITE][ROOK][H1]   = 5;
    PST[OPENING][WHITE][KING][E1]   = 5;
    // Fianchetto
    PST[OPENING][WHITE][BISHOP][B2] = 3;
    PST[OPENING][WHITE][BISHOP][G2] = 3;
    // Protection against bishop attacks
    PST[OPENING][WHITE][PAWN][A3] += 3; 
    PST[OPENING][WHITE][PAWN][H3] += 3;

    // Flip scores according to black's side
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 64; ++j) {
            for (PieceType t = PAWN; t <= KING; t = PieceType(t + 1)) {
                Square ws = board.get_square(j);
                Square bs = board.flip(ws);
                PST[i][BLACK][t][bs] = PST[i][WHITE][t][ws];
            }
        }
    }
    
    // Print PST
    /*
    cout << endl << "Opening pawn PST";
    string squares[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i) {
        Square s = Square(i);
        ostringstream stream;
        stream << setw(4) << PST[OPENING][WHITE][PAWN][s];
        squares[i] = stream.str();
    }
    cout << endl << board.to_string(squares, 4);

    cout << endl << "Ending pawn PST";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        Square s = Square(i);
        ostringstream stream;
        stream << setw(4) << PST[ENDING][WHITE][PAWN][s];
        squares[i] = stream.str();
    }
    cout << endl << board.to_string(squares, 4);
    
    cout << endl << "Opening knight PST";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        Square s = Square(i);
        ostringstream stream;
        stream << setw(4) << PST[OPENING][WHITE][KNIGHT][s];
        squares[i] = stream.str();
    }
    cout << endl << board.to_string(squares, 4);
    */
}

static const int LAZY_EVAL_MARGIN = PIECE_VALUE[ROOK];

int Game::eval(int alpha, int beta) {
    // Material evaluation
    int score = material_eval();

    // TODO Draws should be catched here
    // if (score == 0) return 0; // Draw
    if (score > PIECE_VALUE[KING]) return INF; // Win
    if (score < -PIECE_VALUE[KING]) return -INF; // Loss
   
    // Lazy evaluation
    if (score + LAZY_EVAL_MARGIN < alpha) return score;
    if (score - LAZY_EVAL_MARGIN > beta) return score;

    // TODO Positionnal evaluation
    score += position_eval();

    // TODO Mobility evaluation
    //score += mobility_eval();
    
    return score;
}

int Game::material_eval() {
    Color c;
    int score = 0;
    Position& pos = current_position();
    
    // TODO Enable material hash table
    /*
    // Lookup in hash table
    bool is_empty = true;
    int hash_score = material_table.lookup(pos.material_hash(), is_empty);
    if (!is_empty) {
        c = pos.get_turn_color();
        return (c == WHITE ? hash_score : -hash_score);
    }
    */
    
    int material_score[2] = { 0 };
    int material_bonus[2] = { 0 };
    for (int i = 0; i < 2; ++i) {
        c = Color(i);
        int nb_pawns = 0;
        int nb_minors = 0;
        for (PieceType t = PAWN; t <= KING; t = PieceType(t + 1)) {
            int n = pieces.get_nb_pieces(c, t);
            // Pieces' standard alues
            material_score[c] += n * PIECE_VALUE[t]; 
            
            // Bonus values depending on material imbalance
            int adj;
            switch (t) {
                case PAWN:
                    nb_pawns = n;
                    if (n == 0) material_bonus[c] += NO_PAWNS_MALUS;
                    break;
                case KNIGHT:
                    nb_minors = n;
                    if (n > 1) material_bonus[c] += REDUNDANCY_MALUS;
                    
                    // Value adjusted by the number of pawns on the board
                    adj = PAWNS_ADJUSTEMENT[KNIGHT][nb_pawns];
                    material_bonus[c] += n * adj;
                    break;
                case BISHOP:
                    nb_minors += n;
                    // Bishop bonus pair (from +40 to +64):
                    // less than half a pawn when most or all the pawns are on
                    // the board, and more than half a pawn when half or more 
                    // of the pawns are gone. (Kaufman 1999)
                    //
                    // No bonus for two bishops controlling the same color
                    // No bonus for more than two bishops
                    if (n == 2 && !board.is_same_color(
                            pieces.get_position(c, t, 0),
                            pieces.get_position(c, t, 1))) {
                                material_bonus[c] +=
                                    BISHOP_PAIR_BONUS + (3 * 8 - nb_pawns);
                    }

                    // Value adjusted by the number of pawns on the board
                    adj = PAWNS_ADJUSTEMENT[BISHOP][nb_pawns];
                    material_bonus[c] += n * adj;
                    break;
                case ROOK:
                    // Principle of the redundancy (Kaufman 1999)
                    if (n > 1) material_bonus[c] += REDUNDANCY_MALUS;
                    
                    // Value adjusted by the number of pawns on the board
                    adj = PAWNS_ADJUSTEMENT[ROOK][nb_pawns];
                    material_bonus[c] += n * adj;
                    break;
                case QUEEN:
                    if (nb_minors > 1) {
                        // With two or more minor pieces, the queen
                        // equal two rooks. (Kaufman 1999)
                        material_bonus[c] +=
                            (2 * PIECE_VALUE[ROOK]) - (PIECE_VALUE[QUEEN]);
                    }
                    // Value adjusted by the number of pawns on the board
                    adj = PAWNS_ADJUSTEMENT[QUEEN][nb_pawns];
                    material_bonus[c] += n * adj;
                    break;
                default:
                    break;
            }
        }
    }

    // Insufficiant material
    const int K = PIECE_VALUE[KING];
    const int P = PIECE_VALUE[PAWN];
    const int N = PIECE_VALUE[KNIGHT];
    const int B = PIECE_VALUE[BISHOP];
    for (int i = 0; i < 2; ++i) {
        c = Color(i);
        // FIDE rules for draw
        if (material_score[c] == K) {
            if (material_score[!c] == K)         goto return_material_score;
            if (material_score[!c] == K + B)     goto return_material_score;
            if (material_score[!c] == K + N)     goto return_material_score;
            if (material_score[!c] == K + N + N) goto return_material_score;

            // TODO is this duplicate with MALUS_NO_PAWNS?
            int nb_opponent_pawns = pieces.get_nb_pieces(Color(!c), PAWN);
            if (nb_opponent_pawns == 0 && material_score[!c] < K + 4 * P) {
                goto return_material_score;
            }
        }
    }

    c = pos.get_turn_color();
    score = material_score[c] - material_score[Color(!c)];
    score += material_bonus[c] - material_bonus[Color(!c)];

    return_material_score:
        // TODO Enable material hash table
        /*
        hash_score = (c == WHITE ? score : -score);
        material_table.save(pos.material_hash(), hash_score);
        */
        return score;
}

int castling_score(const Position& pos, Color c) {
    int score = 0;
    if (pos.has_castle(c)) {
        score += CASTLE_BONUS;
    }
    else {
        for (PieceType t = QUEEN; t <= KING; t = PieceType(t + 1)) {
            if (!pos.can_castle(c, t)) { // For each side where no castling
                score += BREAKING_CASTLE_MALUS; // is possible, add a malus
            }
        }
    }
    return score;
}

int Game::position_eval() {
    int phase = 0;
    int position_score[2][2] = { { 0 } };
    int pawns_files[2][8] = { { 0 } };
    const Position& pos = current_position();
    Color c;
    for (int i = 0; i < 2; ++i) {
        c = Color(i);
        for (PieceType t = PAWN; t <= KING; t = PieceType(t + 1)) {
            int n = pieces.get_nb_pieces(c, t);
            phase += n * PHASE_COEF[t];
            for (int j = 0; j < n; ++j) {
                Square s = pieces.get_position(c, t, j);
                position_score[OPENING][c] += PST[OPENING][c][t][s];
                position_score[ENDING][c] += PST[ENDING][c][t][s];
                if (t == PAWN) pawns_files[c][board.get_file(s)]++;
            }
        }

        int pawns_score = 0;
        for (int j = 0; j < 8; ++j) {
            pawns_score += MULTI_PAWNS_MALUS[pawns_files[c][j]];
        }
        position_score[OPENING][c] += pawns_score;
        
        // Rooks' files bonus
        int rooks_score = 0;
        int nb_rooks = pieces.get_nb_pieces(c, ROOK);
        for (int j = 0; j < nb_rooks; ++j) {
            Square s = pieces.get_position(c, ROOK, j);
            if (!pawns_files[!c][board.get_file(s)]) {
                if (!pawns_files[c][board.get_file(s)]) {
                    rooks_score += OPEN_FILE_BONUS;
                }
                else rooks_score += HALF_OPEN_FILE_BONUS;
            }
        }
        position_score[OPENING][c] += rooks_score;

        // Castling bonus/malus
        position_score[OPENING][c] += castling_score(pos, c);

    }
    c = pos.get_turn_color();
    int opening, ending; // Score based on opening and ending rules
    opening = position_score[OPENING][c] - position_score[OPENING][Color(!c)];
    ending = position_score[ENDING][c] - position_score[ENDING][Color(!c)];
    
    // Tapered Eval (idea from Fruit 2.1)
    int max = PHASE_MAX;
    phase = (phase > max ? max : (phase < 0 ? 0 : phase));
    return (opening * phase + ending * (max - phase)) / max;
}
