/* Copyright (C) 2007-2011 Vincent Ollivier
 *
 * Purple Haze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Purple Haze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "game.h"
#include "eval.h"
#include "hashtable.h"

// PST[Phase][Color][PieceType][Square]
static int PST[2][2][NB_PIECE_TYPES][BOARD_SIZE] = { { { { 0 } } } };

void Game::init_eval()
{
    for (const Square &s : SQUARES) {
        for (const PieceType &t : PIECE_TYPES) {
            int opening_score = 0;
            int ending_score = 0;

            switch (t) {
            case PAWN:
                // Develop central pawns
                // But not side pawns
                opening_score = PAWN_FILES_VALUES[board.file(s)];

                // Run for promotion
                ending_score = 10 * board.rank(s);
                break;
            case KNIGHT:
            case BISHOP:
                // Develop toward center files
                opening_score = CENTER_BONUS[board.file(s)];
                if (board.is_border(s)) {
                    opening_score = 2 * BORDER_MALUS;
                }
                // no break
            default:
                ending_score = CENTER_BONUS[board.file(s)];
                ending_score += CENTER_BONUS[board.rank(s)];
                break;
            }

            // Rank bonus
            int bonus = OPENING_RANKS_BONUS[t][board.rank(s)];
            opening_score += (opening_score * bonus) / 2;

            PST[OPENING][WHITE][t][s] = opening_score;
            PST[ENDING][WHITE][t][s] = ending_score;
        }
    }
    // Special corrections
    // Urge to develop light pieces during opening
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
    PST[OPENING][WHITE][PAWN][A3]  += 3;
    PST[OPENING][WHITE][PAWN][H3]  += 3;

    // Flip scores according to black's side
    for (const Phase &p : PHASES) {
        for (const Square &ws : SQUARES) {
            const Square bs = Board::flip(ws);
            for (const PieceType &t : PIECE_TYPES) {
                PST[p][BLACK][t][bs] = PST[p][WHITE][t][ws];
            }
        }
    }
}

static const int LAZY_EVAL_MARGIN = PIECE_VALUE[ROOK];

int Game::eval(int alpha, int beta)
{
    // Material evaluation
    int score = material_eval();

    // TODO Draws should be caught here
    /*
    if (score == 0) {
        return 0; // Draw
    }
    */
    if (score > PIECE_VALUE[KING]) {
        return INF; // Win
    } else if (score < -PIECE_VALUE[KING]) {
        return -INF; // Loss
    }

    // Lazy evaluation
    if (score + LAZY_EVAL_MARGIN < alpha) {
        return score;
    } else if (score - LAZY_EVAL_MARGIN > beta) {
        return score;
    }

    // TODO Positional evaluation
    score += position_eval();

    // TODO Mobility evaluation
    //score += mobility_eval();

    return score;
}

int Game::material_eval()
{
    Position &pos = current_position();

    // Lookup position in material hash table
    bool is_empty = true;
    auto hash_score = material_table.lookup(pos.material_hash(), &is_empty);
    if (!is_empty) {
        const Color c = pos.side();
        return (c == WHITE ? hash_score : -hash_score);
    }

    int scores[2] = { 0 };
    int bonuses[2] = { 0 };
    for (const Color &c : COLORS) {
        int nb_pawns = 0;
        int nb_minors = 0;
        for (const PieceType &t : PIECE_TYPES) {
            const int n = pieces.count(c, t);
            // Standard pieces values
            scores[c] += n * PIECE_VALUE[t];

            // Bonus values depending on material imbalance
            switch (t) {
            case PAWN:
                nb_pawns = n;
                if (n == 0) {
                    bonuses[c] += NO_PAWNS_MALUS;
                }
                break;
            case KNIGHT:
                nb_minors = n;
                if (n > 1) {
                    bonuses[c] += REDUNDANCY_MALUS;
                }

                // Value adjusted by the number of pawns on the board
                bonuses[c] += n * PAWNS_ADJUSTEMENT[KNIGHT][nb_pawns];
                break;
            case BISHOP:
                nb_minors += n;
                // Bishop bonus pair (from +40 to +64): less than half a pawn
                // when most or all the pawns are on the board, and more than
                // half a pawn when half or more of the pawns are gone.
                // (Kaufman 1999)
                //
                // No bonus for two bishops controlling the same color
                // No bonus for more than two bishops
                if (n == 2 && has_bishop_pair(c, pieces)) {
                    bonuses[c] += BISHOP_PAIR_BONUS + 3 * 8 - nb_pawns;
                }

                // Value adjusted by the number of pawns on the board
                bonuses[c] += n * PAWNS_ADJUSTEMENT[BISHOP][nb_pawns];
                break;
            case ROOK:
                // Principle of the redundancy (Kaufman 1999)
                if (n > 1) {
                    bonuses[c] += REDUNDANCY_MALUS;
                }

                // Value adjusted by the number of pawns on the board
                bonuses[c] += n * PAWNS_ADJUSTEMENT[ROOK][nb_pawns];
                break;
            case QUEEN:
                if (nb_minors > 1) {
                    // With two or more minor pieces, the queen equals two
                    // rooks. (Kaufman 1999)
                    bonuses[c] += 2 * PIECE_VALUE[ROOK] - PIECE_VALUE[QUEEN];
                }
                // Value adjusted by the number of pawns on the board
                bonuses[c] += n * PAWNS_ADJUSTEMENT[QUEEN][nb_pawns];
                break;
            default:
                break;
            }
        }
    }

    // Draw by insufficient material detection
    bool is_draw = false;
    const int K = PIECE_VALUE[KING];
    const int P = PIECE_VALUE[PAWN];
    const int N = PIECE_VALUE[KNIGHT];
    const int B = PIECE_VALUE[BISHOP];
    for (const Color &c : COLORS) {
        is_draw = true;
        // FIDE rules for draw
        if (scores[c] == K) {
            if (scores[!c] == K) {
                break;
            } else if (scores[!c] == K + B) {
                break;
            } else if (scores[!c] == K + N) {
                break;
            } else if (scores[!c] == K + N + N) {
                break;
            }

            // TODO is this duplicate with MALUS_NO_PAWNS?
            if (!pieces.count(!c, PAWN) && scores[!c] < K + 4 * P) {
                break;
            }
        }
        is_draw = false; // no break happened
    }

    const Color c = pos.side();
    int score = 0;
    if (!is_draw) {
        score = scores[c] - scores[!c];
        score += bonuses[c] - bonuses[!c];
    }

    // Save score to material hash table
    hash_score = (c == WHITE ? score : -score);
    material_table.save(pos.material_hash(), hash_score);

    return score;
}

static int castling_score(const Position &pos, Color c)
{
    int score = 0;
    if (pos.has_castled(c)) {
        score += CASTLE_BONUS;
    } else {
        for (const PieceType &t : SIDE_TYPES) { // for QUEEN and KING side
            if (!pos.can_castle(c, t)) {
                score += BREAKING_CASTLE_MALUS;
            }
        }
    }
    return score;
}

int Game::position_eval()
{
    int phase = 0;
    int pos_scores[2][2] = { { 0 } };
    int pawns_files[2][8] = { { 0 } };
    const Position &pos = current_position();
    for (const Color &c : COLORS) {
        for (const PieceType &t : PIECE_TYPES) {
            const int n = pieces.count(c, t);
            phase += n * PHASE_COEF[t];
            for (int i = 0; i < n; ++i) {
                const Square s = pieces.position(c, t, i);
                pos_scores[OPENING][c] += PST[OPENING][c][t][s];
                pos_scores[ENDING][c] += PST[ENDING][c][t][s];
                if (t == PAWN) {
                    pawns_files[c][board.file(s)]++;
                }
            }
        }

        int pawns_score = 0;
        for (int i = 0; i < 8; ++i) {
            pawns_score += MULTI_PAWNS_MALUS[pawns_files[c][i]];
        }
        pos_scores[OPENING][c] += pawns_score;

        // Rooks' files bonus
        int rooks_score = 0;
        for (int i = 0, n = pieces.count(c, ROOK); i < n; ++i) {
            const Square s = pieces.position(c, ROOK, i);
            if (!pawns_files[!c][board.file(s)]) {
                if (!pawns_files[c][board.file(s)]) {
                    rooks_score += OPEN_FILE_BONUS;
                } else {
                    rooks_score += HALF_OPEN_FILE_BONUS;
                }
            }
        }
        pos_scores[OPENING][c] += rooks_score;

        // Castling bonus/malus
        pos_scores[OPENING][c] += castling_score(pos, c);

    }

    // Retrieve opening and ending score
    const Color c = pos.side();
    const int opening = pos_scores[OPENING][c] - pos_scores[OPENING][!c];
    const int ending = pos_scores[ENDING][c] - pos_scores[ENDING][!c];

    // Tapered Eval (idea from Fruit 2.1)
    const int max = PHASE_MAX;
    phase = ((phase > max) ? max : ((phase < 0) ? 0 : phase));
    return (opening * phase + ending * (max - phase)) / max;
}
