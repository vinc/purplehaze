#include "../../src/moves.h"
#include "../../src/board.h"
#include "../../src/position.h"
#include "gtest/gtest.h"

TEST(MoveListTest, Size)
{
    int list_size = (MAX_PLY * MAX_BF) * sizeof(ExtendedMove);
    int size = list_size + sizeof(unsigned int);

    // MoveList's internal array's size should not be a power of two
    //EXPECT_NE(0, list_size & (list_size - 1));

    EXPECT_EQ(128 * 256 * 4, list_size);
    EXPECT_EQ(list_size + 4, size);
    EXPECT_EQ(size, sizeof(MoveList));
}

TEST(MoveListTest, Constructor)
{
    MoveList moves;
    for (int i = 0; i < MAX_PLY; ++i) {
        for (int j = 0; j < MAX_BF; ++j) {
            // Test default move
            EXPECT_TRUE(moves[j].is_null());

            // Test default score
            EXPECT_EQ(0, moves[j].value());
        }
        moves.inc_ply();
    }
}

TEST(MoveListTest, Assignement)
{
    MoveList moves;
    Move m;
    for (int i = 0; i < MAX_PLY; ++i) {
        for (int j = 0; j < MAX_BF; ++j) {
            ExtendedMove em1(m, i + j);
            moves[j] = em1;
            EXPECT_EQ(em1, moves[j]);
            EXPECT_EQ(em1.value(), moves[j].value());
        }
        moves.inc_ply();
    }
    for (int i = MAX_PLY - 1; i > 0; --i) {
        moves.dec_ply();
        for (int j = 0; j < MAX_BF; ++j) {
            ExtendedMove em1(m, i + j);
            EXPECT_EQ(em1, moves[j]);
            EXPECT_EQ(em1.value(), moves[j].value());
        }
    }
    moves.clear();
    for (int i = 0; i < MAX_PLY; ++i) {
        for (int j = 0; j < MAX_BF; ++j) {
            ExtendedMove em1(m, i + j);
            EXPECT_EQ(em1, moves[j]);
            EXPECT_EQ(em1.value(), moves[j].value());
        }
        moves.inc_ply();
    }
}


TEST(MovesTest, Size)
{
    int size =
        MOVES_STATE_SIZE * sizeof(unsigned char) + // 5 bytes
        3 +                                        // 3 bytes (padding)
        sizeof(MoveList*) +                        // 4 bytes
        sizeof(Position*) +                        // 4 bytes
        sizeof(Board*) +                           // 4 bytes
        sizeof(Pieces*) +                          // 4 bytes
        2 * sizeof(int) +                          // 8 bytes
        sizeof(MovesState) +                       // 1 byte
        sizeof(bool);                              // 1 byte
    EXPECT_EQ(36, size);
    EXPECT_EQ(size, sizeof(Moves));
}

TEST(MovesTest, Constructor)
{
    Board board;
    Pieces pieces;
    Position position;
    MoveList list;

    Move m;
    ExtendedMove em(m, 50);
    list[0] = em;
    EXPECT_EQ(em.value(), list[0].value());

    EXPECT_EQ(0, list.cur_ply());

    Moves moves(board, pieces, position, list);

    // Test if MoveList::inc_ply() has been implicitly called by Moves()
    EXPECT_EQ(1, list.cur_ply());

    EXPECT_EQ(BEST, moves.state());

    EXPECT_EQ(0, moves.count(BEST));
    EXPECT_EQ(0, moves.count(GOOD_CAPTURES));
    EXPECT_EQ(0, moves.count(KILLERS));
    EXPECT_EQ(0, moves.count(BAD_CAPTURES));
    EXPECT_EQ(0, moves.count(QUIET_MOVES));

    for (int i = 0; i < MAX_PLY; ++i) {
        EXPECT_EQ(0, list[i].value());
    }
    list.dec_ply();
    EXPECT_EQ(em.value(), list[0].value());
}

TEST(MovesTest, Score)
{
    Board board;
    Pieces pieces;
    Position position;
    MoveList list;
    Moves moves(board, pieces, position, list);

    Moves::init_mvv_lva_scores();
    for (const PieceType& v : PIECE_TYPES) {
        for (const PieceType& a : PIECE_TYPES) {
            Square to = E5;
            Square from;
            switch (a) {
                case KNIGHT:
                    from = D3;
                    break;
                default:
                    from = D4;
                    break;
            }
            board[from] = Piece(WHITE, a);
            board[to] = Piece(BLACK, v);
            Move capture(from, to, CAPTURE);
            Score score = moves.mvv_lva_score(capture);

            EXPECT_GT(BEST_SCORE, score);
            EXPECT_LT(KILLERS_SCORE, score);

            for (const PieceType& v2 : PIECE_TYPES) {
                for (const PieceType& a2 : PIECE_TYPES) {
                    switch (a2) {
                        case KNIGHT:
                            from = D3;
                            break;
                        default:
                            from = D4;
                            break;
                    }
                    board[from] = Piece(WHITE, a2);
                    board[to] = Piece(BLACK, v2);
                    Move capture2(from, to, CAPTURE);
                    Score score2 = moves.mvv_lva_score(capture2);

                    if (v > v2) {
                        EXPECT_GT(score, score2);
                    } else if (v < v2) {
                        EXPECT_LT(score, score2);
                    } else {
                        if (a > a2) {
                            EXPECT_LT(score, score2);
                        } else if (a < a2) {
                            EXPECT_GT(score, score2);
                        } else {
                            EXPECT_EQ(score, score2);
                        }
                    }
                }
            }
        }
    }
}

// TODO Add test for Moves destructor (implicit call of MoveList::dec_ply())
