/*  PurpleHaze 1.0
    Copyright (C) 2007-2009  Vincent Ollivier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <vector>
#include <string>
#include <sstream>

#include "common.h"

#ifndef BOOK_H
#define BOOK_H

class Book
{
	private:
		typedef vector<string> Line;
		vector<Line> lines;
		Line current_line;
		
	public:
		
		Book();
		void add_line(string str_line);
		void add_move(string move);
		string get_move() const;
};

#endif /* !BOOK_H */
