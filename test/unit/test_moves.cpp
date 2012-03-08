#include <fstream>

#include "../../src/moves.h"
#include "../../src/board.h"
#include "../../src/position.h"
#include "gtest/gtest.h"

TEST(MoveListTest, Size) {
    int list_size = (MAX_PLY * MAX_BF) * sizeof(ExtendedMove);
    int size = list_size + sizeof(unsigned int);
    
    // MoveList's internal array's size should not be a power of two
    //EXPECT_NE(0, list_size & (list_size - 1));
    
    EXPECT_EQ(size, sizeof(MoveList));
}

TEST(MoveListTest, Constructor) {
    MoveList moves;
    for (int i = 0; i < MAX_PLY; ++i) {
        for (int j = 0; j < MAX_BF; ++j) {
            // Test default move
            EXPECT_TRUE(moves[j].is_null());

            // Test default score
            EXPECT_EQ(0, moves[j].get_score());
        }
        moves.inc_ply();
    }
}

TEST(MoveListTest, Assignement) {
    MoveList moves;
    Move m;
    for (int i = 0; i < MAX_PLY; ++i) {
        for (int j = 0; j < MAX_BF; ++j) {
            ExtendedMove em1(m, i + j);
            moves[j] = em1;
            EXPECT_EQ(em1, moves[j]);
            EXPECT_EQ(em1.get_score(), moves[j].get_score());
        }
        moves.inc_ply();
    }
    for (int i = MAX_PLY - 1; i > 0; --i) {
        moves.dec_ply();
        for (int j = 0; j < MAX_BF; ++j) {
            ExtendedMove em1(m, i + j);
            EXPECT_EQ(em1, moves[j]);
            EXPECT_EQ(em1.get_score(), moves[j].get_score());
        }
    }
    moves.clear();
    for (int i = 0; i < MAX_PLY; ++i) {
        for (int j = 0; j < MAX_BF; ++j) {
            ExtendedMove em1(m, i + j);
            EXPECT_EQ(em1, moves[j]);
            EXPECT_EQ(em1.get_score(), moves[j].get_score());
        }
        moves.inc_ply();
    }
}


TEST(MovesTest, Size) {
    int size =
        MOVES_STATE_SIZE * sizeof(unsigned char) + // 5 bytes
        3 +                                        // 3 bytes (padding)
        sizeof(MoveList*) +                        // 4 bytes
        sizeof(Position*) +                        // 4 bytes
        sizeof(Board*) +                           // 4 bytes
        sizeof(Pieces*) +                          // 4 bytes
        2 * sizeof(unsigned char) +                // 2 bytes
        sizeof(MovesState) +                       // 1 byte
        sizeof(bool);                              // 1 byte
    EXPECT_EQ(size, sizeof(Moves));
}

TEST(MovesTest, Constructor) {
    Position position;
    MoveList list;
    Board board;
    Pieces pieces;

    Move m;
    ExtendedMove em(m, 50);
    list[0] = em;
    EXPECT_EQ(em.get_score(), list[0].get_score());
    
    Moves moves(board, pieces, position, list);

    EXPECT_EQ(BEST, moves.get_state());

    // Test if MoveList::inc_ply() has been implicitly called by Moves()
    for (int i = 0; i < MAX_PLY; ++i) {
        EXPECT_EQ(0, list[i].get_score());
    }
    list.dec_ply();
    EXPECT_EQ(em.get_score(), list[0].get_score());
}
