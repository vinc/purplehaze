#include "../../src/time.h"
#include "gtest/gtest.h"

TEST(TimeTest, Constructor)
{
    // Default constructor
    Time t1;
    const int moves = 40;
    const int time = 5 * 60 * 100; // 5 minutes in centiseconds
    EXPECT_EQ(time / moves, t1.allocated());

    // Explicit constructor
    Time t2(moves, time);
    EXPECT_EQ(time / moves, t2.allocated());
}

TEST(TimeTest, Thinking)
{
    const int level_moves = 40;
    const int level_time = 5 * 60 * 100; // 5 minutes in centiseconds
    const int time_per_move = level_time / level_moves;
    Time t(level_moves, level_time);

    int time = level_time;
    for (int ply = 0; ply < 200; ++ply) {
        t.set_remaining(time);
        t.start_thinking(ply);
        EXPECT_EQ(time_per_move, t.allocated());

        if (ply % 2 != 0) {
            if (((ply / 2) + 1) % level_moves == 0) {
                time = level_time;
            } else {
                time -= time_per_move;
            }
        }
    }
}

// TODO: Add polling and abort tests
