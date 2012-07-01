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

#include <iostream>
#include <cassert>
#include <sstream>
#include <string>

#include "tt.h"

std::string Transposition::to_string() const
{
    std::ostringstream stream;
    stream << "<";
    stream << ", " << std::dec << value() << ", " << depth();
    stream << ", " << bound() << ", " << best_move() << ">";
    return stream.str();
}

template class HashTable<Transposition>; // To avoid linker errors
