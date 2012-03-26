#include "../../src/protocol.h"
#include "gtest/gtest.h"

class SearchMoveTest : public testing::Test
{
    protected:
        Protocol proto;
        std::string fen;

        virtual void SetUp() {
            proto.new_game();
            //proto.set_output_thinking(true);
        }

        std::string search(int time = 10) {
            proto.set_board(fen);
            proto.set_time(1, time);
            return proto.search_move(true);
        }
};

TEST_F(SearchMoveTest, DISABLED_Fine70Position)
{
    fen = "8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - -";
    EXPECT_EQ("Kb1", search());
}

TEST_F(SearchMoveTest, RetiEndgamePosition)
{
    fen = "7K/8/k1P5/7p/8/8/8/8 w - -";
    EXPECT_EQ("Kg7", search());
}

TEST_F(SearchMoveTest, Zugzwang1Position)
{
    fen = "8/8/p1p5/1p5p/1P5p/8/PPP2K1p/4R1rk w - - 0 1";
    EXPECT_EQ("Rf1", search());
}

TEST_F(SearchMoveTest, Zugzwang2Position)
{
    fen = "1q1k4/2Rr4/8/2Q3K1/8/8/8/8 w - - 0 1";
    EXPECT_EQ("Kh6", search());
}

TEST_F(SearchMoveTest, BishopPromotionPosition)
{
    fen = "1q6/P6k/8/5N1K/8/8/8/8 w - - 0 1";
    EXPECT_EQ("axb8=B", search());
}

TEST_F(SearchMoveTest, CastlePosition)
{
    fen = "r1bqkb1r/1ppp1ppp/p1n2n2/4p3/B3P3/5N2/PPPP1PPP/RNBQK2R w KQkq -";
    EXPECT_EQ("O-O", search());
}

TEST_F(SearchMoveTest, Mate1Position)
{
    // Mate in 1
    fen = "8/2r4p/p4p1Q/1p2bp2/8/6k1/P5P1/3B2K1 w - -";
    EXPECT_EQ("Qh2+", search());
}

TEST_F(SearchMoveTest, Mate2Position)
{
    // Mate in 3
    fen = "7k/1p5p/5b2/p3pr2/4n3/4B1rP/PP1N3K/3R1R2 b - -";
    EXPECT_EQ("Rh5", search());
}

TEST_F(SearchMoveTest, Lose1Position)
{
    // Lose in 2
    fen = "7k/1p5p/5b2/p3p2r/4n3/4B1rP/PP1N3K/3R1R2 w - -";
    EXPECT_EQ("Nxe4", search());
}
