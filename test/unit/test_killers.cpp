#include "../../src/game.h"
#include "gtest/gtest.h"

class KillersTest : public testing::Test
{
    protected:
        Game game;
};

TEST_F(KillersTest, Constructor)
{
    for (int d = 0; d < MAX_PLY; ++d) {
        int n = 0;
        for (const Move &killer_move : game.killers(d)) {
            EXPECT_EQ(Move(), killer_move);
            ++n;
        }
        EXPECT_EQ(MAX_KILLERS, n);
    }
}

TEST_F(KillersTest, SetKiller)
{
    for (int d = 0; d < MAX_PLY; ++d) {
        for (int i = 0; i < MAX_KILLERS; ++i) {
            Square to = static_cast<Square>(E4 + i);
            game.set_killer(Move(E2, to, QUIET_MOVE), d);
        }
    }
    for (int d = 0; d < MAX_PLY; ++d) {
        int i = MAX_KILLERS - 1;
        for (const Move &killer_move : game.killers(d)) {
            Square to = static_cast<Square>(E4 + i);
            EXPECT_EQ(Move(E2, to, QUIET_MOVE), killer_move);
            EXPECT_NE(Move(), killer_move);
            --i;
        }
    }
}

TEST_F(KillersTest, ClearKiller)
{
    for (int d = 0; d < MAX_PLY; ++d) {
        for (int i = 0; i < MAX_KILLERS; ++i) {
            Square to = static_cast<Square>(E4 + i);
            game.set_killer(Move(E2, to, QUIET_MOVE), d);
        }
    }
    game.clear_killers();
    for (int d = 0; d < MAX_PLY; ++d) {
        for (const Move &killer_move : game.killers(d)) {
            EXPECT_EQ(Move(), killer_move);
        }
    }
}

TEST_F(KillersTest, IsKiller)
{
    for (int d = 0; d < MAX_PLY; ++d) {
        for (int i = 0; i < MAX_KILLERS; ++i) {
            Square to = static_cast<Square>(E4 + i);
            game.set_killer(Move(E2, to, QUIET_MOVE), d);
        }
    }
    for (int d = 0; d < MAX_PLY; ++d) {
        EXPECT_FALSE(game.is_killer(Move(E2, E3, QUIET_MOVE), d));
        EXPECT_FALSE(game.is_killer(Move(), d));
        for (int i = 0; i < MAX_KILLERS; ++i) {
            Square to = static_cast<Square>(E4 + i);
            EXPECT_TRUE(game.is_killer(Move(E2, to, QUIET_MOVE), d));
        }
    }
}
