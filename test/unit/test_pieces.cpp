#include "../../src/pieces.h"
#include "gtest/gtest.h"

TEST(PiecesTest, Constructor)
{
    Pieces pieces;
    for (const Color &c : COLORS) {
        EXPECT_EQ(0, pieces.count(c));
        for (const PieceType &t : PIECE_TYPES) {
            EXPECT_EQ(0, pieces.count(c, t));
            for (int i = 0; i < 9; ++i) {
                Piece p(c, t, i);
                EXPECT_EQ(OUT, pieces.get_position(p));
                EXPECT_EQ(OUT, pieces.get_position(c, t, i));
            }
        }
    }
}

TEST(PiecesTest, Positions)
{
    Pieces pieces;
    for (const Color &c : COLORS) {
        for (const PieceType &t : PIECE_TYPES) {
            for (int i = 0; i < 9; ++i) {
                for (const Square &s : SQUARES) {
                    Piece p(c, t, i);

                    // Test getters and setters
                    pieces.set_position(p, s);
                    EXPECT_EQ(s, pieces.get_position(p));
                    EXPECT_EQ(s, pieces.get_position(c, t, i));
                    pieces.set_position(c, t, i, s);
                    EXPECT_EQ(s, pieces.get_position(p));
                    EXPECT_EQ(s, pieces.get_position(c, t, i));

                    // Internal counters are not implicitly changed
                    EXPECT_EQ(0, pieces.count(c));
                    EXPECT_EQ(0, pieces.count(c, t));
                }
            }
        }
    }
}

TEST(PiecesTest, Count)
{
    Pieces pieces;
    for (const Color &c : COLORS) {
        EXPECT_EQ(0, pieces.count(c));
        for (const PieceType &t : PIECE_TYPES) {
            EXPECT_EQ(0, pieces.count(c, t));
            for (int i = 0; i < 9; ++i) {
                EXPECT_EQ(i, pieces.count(c, t));
                pieces.inc_nb_pieces(c, t);
                EXPECT_EQ(i + 1, pieces.count(c, t));
            }
            for (int i = 9; i > 0; --i) {
                EXPECT_EQ(i, pieces.count(c, t));
                pieces.dec_nb_pieces(c, t);
                EXPECT_EQ(i - 1, pieces.count(c, t));
            }
            EXPECT_EQ(0, pieces.count(c, t));
        }
        EXPECT_EQ(0, pieces.count(c));
    }
}

TEST(PiecesTest, Size)
{
    int size = (sizeof(Square) * 2 * 7 * 9) +
               (sizeof(unsigned char) * 2 * 7) +
               (sizeof(unsigned char) * 2);
    EXPECT_EQ(size, 142); // Square is unsigned char (1 byte)
    EXPECT_EQ(size, sizeof(Pieces));
}
