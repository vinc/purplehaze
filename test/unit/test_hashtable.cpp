#include <climits>

#include "../../src/hashtable.h"
#include "gtest/gtest.h"

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
