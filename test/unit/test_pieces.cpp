#include "../../src/pieces.h"
#include "../../src/game.h"
#include "gtest/gtest.h"

TEST(PiecesTest, Constructor)
{
    Pieces pieces;
    for (const Color& c : COLORS) {
        EXPECT_EQ(0, pieces.count(c));
        for (const PieceType& t : PIECE_TYPES) {
            EXPECT_EQ(0, pieces.count(c, t));
            for (int i = 0; i < 9; ++i) {
                Piece p(c, t, i);
                EXPECT_EQ(OUT, pieces.position(p));
                EXPECT_EQ(OUT, pieces.position(c, t, i));
            }
        }
    }
}

TEST(PiecesTest, Positions)
{
    Pieces pieces;
    for (const Color& c : COLORS) {
        for (const PieceType& t : PIECE_TYPES) {
            for (int i = 0; i < 9; ++i) {
                for (const Square& s : SQUARES) {
                    Piece p(c, t, i);

                    // Test getters and setters
                    pieces.set_position(p, s);
                    EXPECT_EQ(s, pieces.position(p));
                    EXPECT_EQ(s, pieces.position(c, t, i));
                    pieces.set_position(c, t, i, s);
                    EXPECT_EQ(s, pieces.position(p));
                    EXPECT_EQ(s, pieces.position(c, t, i));

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
    for (const Color& c : COLORS) {
        for (const PieceType& t : PIECE_TYPES) {
            EXPECT_EQ(0, pieces.count(c));
            EXPECT_EQ(0, pieces.count(c, t));
            for (int i = 0; i < 9; ++i) {
                EXPECT_EQ(i, pieces.count(c));
                EXPECT_EQ(i, pieces.count(c, t));
                pieces.inc_nb_pieces(c, t);
                EXPECT_EQ(i + 1, pieces.count(c));
                EXPECT_EQ(i + 1, pieces.count(c, t));
            }
            for (int i = 9; i > 0; --i) {
                EXPECT_EQ(i, pieces.count(c));
                EXPECT_EQ(i, pieces.count(c, t));
                pieces.dec_nb_pieces(c, t);
                EXPECT_EQ(i - 1, pieces.count(c));
                EXPECT_EQ(i - 1, pieces.count(c, t));
            }
            EXPECT_EQ(0, pieces.count(c));
            EXPECT_EQ(0, pieces.count(c, t));
        }
    }
}

TEST(PiecesTest, CountBoard)
{
    Game game;
    game.fen("8/8/1k6/ppp4P/6P1/5P2/4P3/4K3 w - - 0 1");
    EXPECT_EQ(5, game.pieces.count(WHITE));
    EXPECT_EQ(4, game.pieces.count(WHITE, PAWN));
    EXPECT_EQ(1, game.pieces.count(WHITE, KING));
    EXPECT_EQ(4, game.pieces.count(BLACK));
    EXPECT_EQ(3, game.pieces.count(BLACK, PAWN));
    EXPECT_EQ(1, game.pieces.count(BLACK, KING));
}


TEST(PiecesTest, Size)
{
    EXPECT_EQ(sizeof(Square), sizeof(uint8_t));
    int size =
        sizeof(Square) * 2 * 7 * 9 +    // 126 bytes
        sizeof(uint8_t) * 2 * 7 +       // 14 bytes
        sizeof(uint8_t) * 2;            // 2 bytes
    EXPECT_EQ(size, sizeof(Pieces));
}
