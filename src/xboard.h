/* Copyright (C) 2007-2011 Vincent Ollivier
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

#ifndef XBOARD_H
#define XBOARD_H

#include <iostream>
#include <fstream>
#include <string>

#include "protocol.h"

static const int XBOARD_NB_FEATURES = 3;
static const string XBOARD_FEATURES[XBOARD_NB_FEATURES][2] = {
    { "myname",   "Purple Haze " + string(VERSION) },
    { "setboard", "1" },
    { "done",     "1" },
};

class Xboard : public Protocol
{
    private:
        ofstream log;
        bool force_mode;
    public:
        Xboard();
        void loop();
        void think();
};

#endif /* !XBOARD_H */
