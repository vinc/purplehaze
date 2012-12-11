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

#ifndef TREE_H
#define TREE_H

#include "position.h"

static const unsigned int MAX_TREE = 1024;

class Tree
{
    private:
        Position tree[MAX_TREE];
        int offset;
        int size;
    public:
        Tree() : offset(0), size(0) {}
        void push() {
            tree[size + 1] = tree[size];
            ++size;
        };
        void pop() {
            --size;
        };
        Position& top() {
            return tree[size];
        };
        int ply() const {
            return offset + size;
        };
        void set_ply(int o) {
            offset = o;
        };
        bool has_repetition_draw();
};

#endif /* !TREE_H */
