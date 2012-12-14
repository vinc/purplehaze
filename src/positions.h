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

#ifndef POSITIONS_H
#define POSITIONS_H

#include "position.h"

template <class T>
class Stack
{
protected:
    static const int MAX = 1024;
    T stack[MAX];
    int size;
public:
    Stack() : size(0) {}
    void push(T& t) {
        stack[++size] = t;
    }
    void pop() {
        --size;
    }
    T& top() {
        return stack[size];
    }
};

class Positions : public Stack<Position>
{
private:
    int offset;
public:
    Positions() : Stack<Position>(), offset(0) {}

    // Renamed stack methods
    Position& current() {
        return top();
    }
    void clone_current() {
        push(top());
    }
    void delete_current() {
        pop();
    }

    // Specific methods
    int ply() const {
        return offset + size;
    }
    void set_ply(int o) {
        offset = o;
    }
    bool is_draw();
};

#endif /* !POSITIONS_H */
