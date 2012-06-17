#include "../../src/board.h"
#include "../../src/move.h"
#include "../../src/piece.h"
#include "gtest/gtest.h"

TEST(BoardTest, Constructor)
{
    Board board;

    // Test empty board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        const Square s = static_cast<Square>(i);
        EXPECT_EQ(Piece(), board[s]);
    }

    // Test attack and direction arrays
    for (const Square &from : SQUARES) {
        if (from == OUT) {
            break;
        }
        for (const Square &to : SQUARES) {
            if (to == OUT) {
                break;
            }
            Direction dir = NO_DIR;
            for (const PieceType& t : NOT_PAWN_TYPES) {
                bool can_attack = false;
                for (const Direction &d : PIECES_DIRS[t]) {
                    if (d == NO_DIR) {
                        break;
                    }
                    Square s = static_cast<Square>(from + d);
                    while (!board.is_out(s)) {
                        if (s == to) {
                            can_attack = true;
                            dir = d;
                            break;
                        }
                        switch (t) {
                        case KNIGHT:
                        case KING:
                            s = OUT;
                            break;
                        default:
                            s = static_cast<Square>(s + d);
                            break;
                        }
                    }
                }
                if (can_attack) {
                    EXPECT_TRUE(board.can_attack(t, from, to));
                } else {
                    EXPECT_FALSE(board.can_attack(t, from, to));
                }
            }
            EXPECT_EQ(dir, board.direction_to(from, to));
        }
    }
}
