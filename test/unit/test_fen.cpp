#include "../../src/common.h"
#include "../../src/game.h"
#include "gtest/gtest.h"

class FenTest : public testing::Test
{
    protected:
        Game game;
        std::string fen;
};

TEST_F(FenTest, StartingPositions)
{
    // Starting position
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    game.fen(fen);
    EXPECT_EQ(fen, game.fen());

    // After move 1. e4
    fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    game.fen(fen);
    EXPECT_EQ(fen, game.fen());

    // After move 1. ... c5
    fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2";
    game.fen(fen);
    EXPECT_EQ(fen, game.fen());
    
    // After move 2. Nf3
    fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
    game.fen(fen);
    EXPECT_EQ(fen, game.fen());
}

TEST_F(FenTest, NoCastlePosition)
{
    fen = "4k3/8/8/8/8/8/4P3/4K3 w - - 5 39";
    game.fen(fen);
    EXPECT_EQ(fen, game.fen());
}
