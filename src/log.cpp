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
#include <fstream>

#include "log.h"

/*
Log& Log::icmd(Log& log)
{
    std::cout << "bip";
    log << "test";
    log.cout.stream << 123 << "<< ";
    return log;
}
*/

Log& Log::to(Stream s)
{
    switch (s) {
    case COUT:
        cout.state = true;
        file.state = false;
        break;
    case FILE:
        cout.state = false;
        file.state = true;
        break;
    case BOTH:
        cout.state = true;
        file.state = true;
        break;
    }
    return *this;
}

Log& Log::operator<<(const LogDirection& dir)
{
    if (file.state && file.stream.is_open()) {
        switch (dir) {
        case IN:
            file.stream << ">> ";
            break;
        case OUT:
            file.stream << "<< ";
            break;
        case DEBUG:
            file.stream << "## ";
            break;
        }
    }
    return *this;
}
