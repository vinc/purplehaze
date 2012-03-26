#include "../../src/common.h"
#include "../../src/game.h"
#include "gtest/gtest.h"

class PerftTest : public testing::Test
{
    protected:
        Game game;
        std::string fen;
};

TEST_F(PerftTest, StartPosition)
{
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    game.init(fen);
    EXPECT_EQ(20, game.perft(1));
    EXPECT_EQ(400, game.perft(2));

    // Test captures and checks
    EXPECT_EQ(8902, game.perft(3));

    // Test checkmates
    EXPECT_EQ(197281, game.perft(4));

    // Test en passants
    //EXPECT_EQ(4865609, game.perft(5));
}

TEST_F(PerftTest, CastlePosition)
{
    fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    game.init(fen);
    EXPECT_EQ(48, game.perft(1));
    EXPECT_EQ(2039, game.perft(2));
    EXPECT_EQ(97862, game.perft(3));
}

TEST_F(PerftTest, CheckPosition)
{
    fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
    game.init(fen);
    EXPECT_EQ(14, game.perft(1));
    EXPECT_EQ(191, game.perft(2));
    EXPECT_EQ(2812, game.perft(3));
    EXPECT_EQ(43238, game.perft(4));
}

TEST_F(PerftTest, Promotion1Position)
{
    fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    game.init(fen);
    EXPECT_EQ(6, game.perft(1));
    EXPECT_EQ(264, game.perft(2));
    EXPECT_EQ(9467, game.perft(3));
    EXPECT_EQ(422333, game.perft(4));
}

TEST_F(PerftTest, Promotion2Position)
{
    fen = "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1";
    game.init(fen);
    EXPECT_EQ(6, game.perft(1));
    EXPECT_EQ(264, game.perft(2));
    EXPECT_EQ(9467, game.perft(3));
    EXPECT_EQ(422333, game.perft(4));
}

TEST_F(PerftTest, Promotion3Position)
{
    fen = "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1";
    game.init(fen);
    EXPECT_EQ(24, game.perft(1));
    EXPECT_EQ(496, game.perft(2));
    EXPECT_EQ(9483, game.perft(3));
    EXPECT_EQ(182838, game.perft(4));
}

TEST_F(PerftTest, CaptureBug1Position)
{
    fen = "r6R/4k3/8/8/8/8/8/4K2R b K - 0 2";
    game.init(fen);
    EXPECT_EQ(19, game.perft(1));
    EXPECT_EQ(447, game.perft(2)); // Was 446 before bug fix #59cef02
    EXPECT_EQ(7518, game.perft(3));
    //EXPECT_EQ(176792, game.perft(4));
}

TEST_F(PerftTest, MaxMoves1Position)
{
    fen = "3Q4/1Q4Q1/4Q3/2Q4R/Q4Q2/3Q4/1Q4Rp/1K1BBNNk w - - 0 1";
    game.init(fen);
    EXPECT_EQ(218, game.perft(1)); // Need MAX_PLY > 218
    EXPECT_EQ(20, game.perft(2));
    EXPECT_EQ(4072, game.perft(3));
    EXPECT_EQ(15903, game.perft(4));
}

TEST_F(PerftTest, MaxMoves2Position)
{
    fen = "n1r1r1b1/1P1P1P1P/1Q6/3NBNK1/R7/4p1p1/3PBPkP/2R5 w - - 0 1";
    game.init(fen);
    EXPECT_EQ(144, game.perft(1));
    EXPECT_EQ(3283, game.perft(2));
    EXPECT_EQ(407751, game.perft(3));
}

TEST_F(PerftTest, MaxCapturesPosition)
{
    fen = "r1n1n1b1/1P1P1P1P/1N1N1N2/2RnQrRq/2pKp3/3BNQbQ/k7/4Bq2 w - - 0 1";
    game.init(fen);
    EXPECT_EQ(127, game.perft(1));
    EXPECT_EQ(6926, game.perft(2));
    EXPECT_EQ(767038, game.perft(3));
}
