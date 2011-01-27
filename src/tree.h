/* PurpleHaze 2.0.0
 * Copyright (C) 2007-2011  Vincent Ollivier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TREE_H
#define TREE_H

//#include <stack>

#include "node.h"

//using namespace std;

static const unsigned int MAX_TREE = 512;

class Tree
{
    private:
    	//stack<Node> tree;
	Node tree[MAX_TREE];
	unsigned int tree_top; // Redondant with current_node().get_ply()
    public:
	Tree() : tree_top(0) {}
	//Node& top() { return tree.top(); };
	//void push() { tree.push(tree.top()); };	
	//void pop() { tree.pop(); };
	void push() {
	    tree[tree_top + 1] = tree[tree_top];
	    ++tree_top;
	};
	void pop() { --tree_top; };
	Node& top() { return tree[tree_top]; };

};

#endif /* !TREE_H */
