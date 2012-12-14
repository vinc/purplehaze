#include "../../src/piece.h"
#include "gtest/gtest.h"

TEST(PieceTest, Constructor)
{
    for (const Color& c : COLORS) {
        for (const PieceType& t : PIECE_TYPES) {
            for (int i = 0; i < 9; ++i) {
                Piece p(c, t, i);

                // Test getters
                EXPECT_EQ(c, p.color());
                EXPECT_NE(!c, p.color());
                EXPECT_EQ(t, p.type());
                EXPECT_EQ(i, p.index());

                EXPECT_TRUE(p.is(c));
                EXPECT_FALSE(p.is(!c));
                EXPECT_TRUE(p.is(t));
                EXPECT_TRUE(p.is(c, t));
                EXPECT_FALSE(p.is(!c, t));

                // Test comparison operators
                for (const Color& c2 : COLORS) {
                    for (const PieceType& t2 : PIECE_TYPES) {
                        for (int j = 0; j < 9; ++j) {
                            Piece p2(c2, t2, j);
                            if (c == c2 && t == t2 && i == j) {
                                EXPECT_EQ(p2, p);
                            } else {
                                EXPECT_NE(p2, p);
                            }
                        }
                    }
                }
            }
        }
    }
}

TEST(PieceTest, Size)
{
    EXPECT_EQ(sizeof(unsigned char), sizeof(Piece));
}
