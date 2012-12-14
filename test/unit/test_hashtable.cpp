#include <climits>

#include "../../src/hashtable.h"
#include "gtest/gtest.h"

typedef uint64_t Value;

class HashTableTest : public testing::Test
{
protected:
    static const int HT_SIZE = 1024;
    HashTable<Value> ht;

    HashTableTest() : ht(HT_SIZE) {}

    Hash hash_from_index(int i) const {
        return static_cast<Hash>(i & (ht.size() - 1));
    }
};

TEST_F(HashTableTest, Size)
{
    HashTable<Value> ht2(HT_SIZE);
    EXPECT_EQ(sizeof(ht2), sizeof(ht));

    int entry_size = sizeof(Hash) + sizeof(Value);
    EXPECT_EQ(16, entry_size);

    int size = HT_SIZE / entry_size;

    // 'size' must be a power of two for HashTable<T>::lookup()
    EXPECT_EQ(0, size & (size - 1));

    EXPECT_EQ(size, ht.size());
}

TEST_F(HashTableTest, Constructor)
{
    ht.clear();
    const int n = ht.size();
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ(0, ht.hash_at(i));
        EXPECT_EQ(0, ht.value_at(i));
    }
}

TEST_F(HashTableTest, ConstructorWithoutClear)
{
    const int n = ht.size();
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ(0, ht.hash_at(i));
        EXPECT_EQ(0, ht.value_at(i));
    }
}

TEST_F(HashTableTest, Clear)
{
    const int n = ht.size();
    for (int i = 0; i < n; ++i) {
        Hash h = hash_from_index(i);
        ht.save(h, i);

        EXPECT_EQ(h, ht.hash_at(i));
        EXPECT_EQ(i, ht.value_at(i));
    }
    ht.clear();
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ(0, ht.hash_at(i));
        EXPECT_EQ(0, ht.value_at(i));
    }
}

TEST_F(HashTableTest, Lookup)
{
    const int n = ht.size();
    for (int i = 0; i < n; ++i) {
        Hash h = hash_from_index(i);
        bool is_empty;
        Value v = ht.lookup(h, &is_empty);
        EXPECT_EQ(0, v);
        if (i == 0) {
            // FIXME
            EXPECT_FALSE(is_empty);
        } else {
            EXPECT_TRUE(is_empty);
        }
    }
    for (int i = -1; i < n; ++i) {
        for (int s = SHRT_MIN; s <= SHRT_MAX; s += 1000) {
            if (++i >= n) {
                break;
            }

            Value v = static_cast<Value>(s + i);
            Hash h = hash_from_index(i);
            ht.save(h, v);

            // Check save
            EXPECT_EQ(h, ht.hash_at(i));
            EXPECT_EQ(v, ht.value_at(i));
            EXPECT_NE(0, ht.value_at(i));
        }
    }
    for (int i = -1; i < n; ++i) {
        for (int s = SHRT_MIN; s <= SHRT_MAX; s += 1000) {
            if (++i >= n) {
                break;
            }

            Value v = static_cast<Value>(s + i);
            Hash h = hash_from_index(i);
            ht.save(h, v);

            // Test lookup
            bool is_empty;
            Value v2 = ht.lookup(h, &is_empty);
            EXPECT_FALSE(is_empty);
            EXPECT_EQ(v, v2);
        }
    }
}

TEST(MaterialTableTest, Size)
{
    HashTable<Material> ht(MT_SIZE);

    int entry_size = sizeof(Hash) + sizeof(Material);
    EXPECT_EQ(16, entry_size);

    int size = MT_SIZE / entry_size;

    // 'size' must be a power of two for HashTable<Material>::lookup()
    EXPECT_EQ(0, size & (size - 1));

    EXPECT_EQ(size, ht.size());
}
