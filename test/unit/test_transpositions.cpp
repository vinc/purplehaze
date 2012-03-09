#include <fstream>
#include <climits>

#include "../../src/tt.h"
#include "gtest/gtest.h"

static const Bound BOUNDS[] = { EXACT, LOWER, UPPER };

TEST(TranspositionTest, Size) {
    int size = sizeof(short) +          // 2 bytes
               sizeof(Move) +           // 2 bytes
               sizeof(unsigned char) +  // 1 byte
               sizeof(Bound);           // 1 byte
    EXPECT_EQ(6, size);
    EXPECT_EQ(size, sizeof(Transposition));
}

TEST(TranspositionTest, Constructor) {
    Transposition t1;
    EXPECT_TRUE(t1.is_empty());
    EXPECT_EQ(0, t1.get_value());
    EXPECT_EQ(0, t1.get_depth());
    EXPECT_EQ(Move(), t1.get_best_move());
    EXPECT_EQ(UNDEF_BOUND, t1.get_bound());

    for (int v = SHRT_MIN; v <= SHRT_MAX; v += 1000) {
        for (int d = 0; d <= UCHAR_MAX; ++d) {
            for (const Bound &b : BOUNDS) {
                Move m(E2, E3, QUIET_MOVE);
                Transposition t2(v, b, d, m);
                EXPECT_FALSE(t2.is_empty());
                EXPECT_EQ(v, t2.get_value());
                EXPECT_EQ(d, t2.get_depth());
                EXPECT_EQ(m, t2.get_best_move());
                EXPECT_EQ(b, t2.get_bound());
                
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
};

TEST_F(TranspositionsTest, Size) {
    HashTable<Transposition> ht(TT_SIZE);
    EXPECT_EQ(sizeof(ht), sizeof(tt));
    int entry_size = sizeof(Hash) +          // 8 bytes
                     sizeof(Transposition) + // 6 bytes
                     2;                      // 2 bytes (padding)
    int size = TT_SIZE / entry_size;

    // Internal array size should not be a power of two?
    //EXPECT_NE(0, TT_SIZE & (TT_SIZE - 1));
    
    // But internal array entries number should be a power of two
    EXPECT_EQ(0, size & (size - 1));

    EXPECT_EQ(size, tt.size());
}

TEST_F(TranspositionsTest, Constructor) {
    tt.clear();
    int n = tt.size();
    for (int i = 0; i < n; ++i) {
        EXPECT_TRUE(tt.get_value_at(i).is_empty());
        EXPECT_EQ(0, tt.get_hash_at(i));
    }
}

TEST_F(TranspositionsTest, Lookup) {
    tt.clear();
    int n = tt.size();
    for (int i = 0; i < n; ++i) {
        Hash h = static_cast<Hash>(i);
        bool is_empty;
        Transposition trans = tt.lookup(h, &is_empty);
        if (i == 0) {
            // FIXME
            EXPECT_FALSE(is_empty);
        } else {
            EXPECT_TRUE(is_empty);
        }
    }
    /*
    for (int i = 0; i < n; ++i) {
        for (int s = SHRT_MIN; s <= SHRT_MAX; s += 1000) {
            for (int d = 0; d <= UCHAR_MAX; ++d) {
                for (const Bound &b : BOUNDS) {
                    // Create transposition
                    Move m(E2, E4, QUIET_MOVE);
                    int v = s + i;
                    Hash h = static_cast<Hash>(i);
                    Transposition trans_sent(v, b, d, m);

                    // Save transposition
                    tt.save(h, v, b, d, m);

                    // Check transposition
                    EXPECT_FALSE(tt.get_value_at(i).is_empty());
                    EXPECT_EQ(trans_sent, tt.get_value_at(i));
                    EXPECT_EQ(h, tt.get_hash_at(i));

                    ++i;
                }
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int s = SHRT_MIN; s <= SHRT_MAX; s += 1000) {
            for (int d = 0; d <= UCHAR_MAX; ++d) {
                for (const Bound &b : BOUNDS) {
                    // Create transposition
                    Move m(E2, E4, QUIET_MOVE);
                    int v = s + i;
                    Hash h = static_cast<Hash>(i);
                    Transposition trans_sent(v, b, d, m);

                    // Lookup transposition
                    bool is_empty;
                    Transposition trans_received = tt.lookup(h, &is_empty);
                    EXPECT_FALSE(is_empty);
                    EXPECT_EQ(trans_sent, trans_received);

                    ++i;
                }
            }
        }
    }
    */
}
