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

static const unsigned int MAX_TREE = 512;

class Tree
{
    private:
        Position tree[MAX_TREE];
        uint16_t tree_offset;
        uint16_t tree_top;
    public:
        Tree() : tree_offset(0), tree_top(0) {}
        void push() {
            tree[tree_top + 1] = tree[tree_top];
            ++tree_top;
        };
        void pop() {
            --tree_top;
        };
        Position& top() {
            return tree[tree_top];
        };
        uint16_t ply() const {
            return tree_offset + tree_top;
        };
        void set_ply(uint16_t offset) {
            tree_offset = offset;
        };
        bool has_repetition_draw();
};

#endif /* !TREE_H */
