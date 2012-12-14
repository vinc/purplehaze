/* Copyright (C) 2007-2012 Vincent Ollivier
 *
 * Purple Haze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Purple Haze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "common.h"
#include "zobrist.h"

template <class T>
class HashTable
{
protected:
    const int SIZE;
    struct Entry {
        Hash hash;
        T value;

        Entry() : hash(0), value() {}
    };
    Entry* entries;

public:
    struct Stats {
        long hits;
        long collisions;
        long misses;
        Stats() : hits(0), collisions(0), misses(0) {}
    } stats;

    HashTable(int used_space = MT_SIZE) :
        SIZE(used_space / sizeof(Entry)),
        entries(new Entry[SIZE])
        {}
    ~HashTable() {
        delete [] entries;
        entries = NULL;
    };
#ifndef NDEBUG
    T lookup(Hash h, bool* is_empty);
#else
    T lookup(Hash h, bool* is_empty) {
        Entry entry = entries[h & (SIZE - 1)];
        if (entry.hash == h) {
            *is_empty = false;
            return entry.value;
        } else {
            *is_empty = true;
            return T();
        }
    }
#endif
    void save(Hash h, T v) {
        entries[h & (SIZE - 1)].hash = h;
        entries[h & (SIZE - 1)].value = v;
    };
    void clear();

    // Used to print stats
    int size() const {
        return SIZE;
    };

    T value_at(int i) const {
        return entries[i].value;
    };
    Hash hash_at(int i) const {
        return entries[i].hash;
    };

    int usage() const;
};

#endif /* !HASH_TABLE_H */
