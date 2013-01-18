#include <climits>

#include "../../src/tt.h"
#include "../../src/game.h"
#include "gtest/gtest.h"

static const Bound BOUNDS[] = { EXACT, LOWER, UPPER };

TEST(TranspositionTest, Size)
{
    int size = sizeof(int16_t) +        // 2 bytes
               sizeof(Move) +           // 2 bytes
               sizeof(uint8_t) +        // 1 byte
               sizeof(Bound);           // 1 byte
    EXPECT_EQ(6, size);
    EXPECT_EQ(size, sizeof(Transposition));
}

TEST(TranspositionTest, Constructor)
{
    Transposition t1;
    EXPECT_TRUE(t1.is_empty());
    EXPECT_EQ(0, t1.value());
    EXPECT_EQ(0, t1.depth());
    EXPECT_EQ(Move(), t1.best_move());
    EXPECT_EQ(UNDEF_BOUND, t1.bound());

    for (int v = SHRT_MIN; v <= SHRT_MAX; v += 1000) {
        for (int d = 0; d <= UCHAR_MAX; ++d) {
            for (const Bound& b : BOUNDS) {
                Move m(E2, E3, QUIET_MOVE);
                Transposition t2(v, b, d, m);
                EXPECT_FALSE(t2.is_empty());
                EXPECT_EQ(v, t2.value());
                EXPECT_EQ(d, t2.depth());
                EXPECT_EQ(m, t2.best_move());
                EXPECT_EQ(b, t2.bound());

                Transposition t3(v, b, d, m);
                Transposition t4(v + 1, b, d, m);
                Transposition t5(v, UNDEF_BOUND, d, m);
                Transposition t6(v, b, d + 1, m);
                Transposition t7(v, b, d, Move(F2, F3, QUIET_MOVE));

                EXPECT_EQ(t3, t2);
                EXPECT_NE(t4, t2);
                EXPECT_NE(t5, t2);
                EXPECT_NE(t6, t2);
                EXPECT_NE(t7, t2);
            }
        }
    }
}

class TranspositionsTest : public testing::Test
{
protected:
    Transpositions tt;

    Hash hash_from_index(int i) const {
        return static_cast<Hash>(i & (tt.size() - 1));
    }
};

TEST_F(TranspositionsTest, Size)
{
    HashTable<Transposition> ht(TT_SIZE);
    EXPECT_EQ(sizeof(ht), sizeof(tt));
    int entry_size = sizeof(Hash) +          // 8 bytes
                     sizeof(Transposition) + // 6 bytes
                     2;                      // 2 bytes (padding)
    int size = TT_SIZE / entry_size;

    // TODO Internal array size should not be a power of two?
    //EXPECT_NE(0, TT_SIZE & (TT_SIZE - 1));

    // 'size' must be a power of two for Transpositions::lookup()
    EXPECT_EQ(0, size & (size - 1));

    EXPECT_EQ(size, tt.size());
}

TEST_F(TranspositionsTest, Constructor)
{
    tt.clear();
    int n = tt.size();
    for (int i = 0; i < n; ++i) {
        EXPECT_TRUE(tt.value_at(i).is_empty());
        EXPECT_EQ(0, tt.hash_at(i));
    }
}

TEST_F(TranspositionsTest, ConstructorWithoutClear)
{
    int n = tt.size();
    for (int i = 0; i < n; ++i) {
        EXPECT_TRUE(tt.value_at(i).is_empty());
        EXPECT_EQ(0, tt.hash_at(i));
    }
}

TEST_F(TranspositionsTest, Lookup)
{
    tt.clear();
    int n = tt.size();
    for (int i = 0; i < n; ++i) {
        Hash h = hash_from_index(i);
        bool is_empty;
        Transposition trans = tt.lookup(h, &is_empty);
        if (i == 0) {
            // FIXME
            EXPECT_FALSE(is_empty);
            EXPECT_EQ(UNDEF_BOUND, trans.bound());
        } else {
            EXPECT_TRUE(is_empty);
        }
    }
    for (int i = -1; i < n; ++i) {
        for (int s = SHRT_MIN; s <= SHRT_MAX; s += 1000) {
            for (int d = 0; d <= UCHAR_MAX; ++d) {
                for (const Bound& b : BOUNDS) {
                    if (++i >= n) {
                        break;
                    }
                    // Create transposition
                    Move m(E2, E4, QUIET_MOVE);
                    int v = s + i;
                    Hash h = hash_from_index(i);
                    Transposition trans_sent(v, b, d, m);

                    // Save transposition
                    tt.save(h, v, b, d, m);

                    // Check transposition
                    EXPECT_FALSE(tt.value_at(i).is_empty());
                    EXPECT_EQ(trans_sent, tt.value_at(i));
                    EXPECT_EQ(h, tt.hash_at(i));
                }
            }
        }
    }
    for (int i = -1; i < n; ++i) {
        for (int s = SHRT_MIN; s <= SHRT_MAX; s += 1000) {
            for (int d = 0; d <= UCHAR_MAX; ++d) {
                for (const Bound& b : BOUNDS) {
                    if (++i >= n) {
                        break;
                    }
                    // Create transposition
                    Move m(E2, E4, QUIET_MOVE);
                    int v = s + i;
                    Hash h = hash_from_index(i);
                    Transposition trans_sent(v, b, d, m);

                    // Lookup transposition
                    bool is_empty;
                    Transposition trans_received = tt.lookup(h, &is_empty);
                    EXPECT_FALSE(is_empty);
                    EXPECT_EQ(trans_sent, trans_received);
                }
            }
        }
    }
}

/**
 * Test case for retrieving the best move from TT after a search
 */
TEST(TTTest, LookupAfterSearch)
{
    Game game;
    game.tt.clear();
    game.clear_killers();
    game.search_moves.clear();

    game.fen("7K/8/k1P5/7p/8/8/8/8 w - -");
    game.time = Time(1, 200); // Search 1 move in 200 ms
    Move m1 = game.root(MAX_PLY);
    EXPECT_NE("c7", game.output_move(m1)); // BM at depth <= 3
    EXPECT_NE("Kh7", game.output_move(m1)); // BM at depth <= 13
    EXPECT_EQ("Kg7", game.output_move(m1)); // BM at depth >= 13

    bool is_empty;
    Position& pos = game.positions.current();
    Move m2 = game.tt.lookup(pos.hash(), &is_empty).best_move();
    EXPECT_EQ(m1, m2);
}
