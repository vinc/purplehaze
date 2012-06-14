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

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <fstream>

class Log : public std::ostream
{
    private:
        template <class T>
        struct LogStream {
            T stream;
            bool state;

            LogStream() :
                stream(),
                state(false)
                {}
            LogStream(std::streambuf *sb) :
                stream(sb),
                state(true)
                {}
            LogStream(std::string filename) :
                stream(filename, std::ios::app),
                state(true)
                {}
        };
        LogStream<std::ostream> cout;
        LogStream<std::ofstream> file;

    public:
        enum Stream { COUT, FILE, BOTH };
        enum LogDirection { IN, OUT, DEBUG };

        Log() :
            std::ostream(std::cout.rdbuf()),
            cout(std::cout.rdbuf()),
            file()
            {}

        Log(std::string filename) :
            std::ostream(std::cout.rdbuf()),
            cout(std::cout.rdbuf()),
            file(filename)
            {}

        void open(std::string filename) {
            file.stream.open(filename, std::ios::app);
            file.state = true;
        };

        template <typename T>
        Log& operator<<(const T &val) {
            if (cout.state) {
                cout.stream << val;
            }
            if (file.state && file.stream.is_open()) {
                file.stream << val;
            }
            return *this;
        }

        Log& operator<<(std::ostream& (*manip)(std::ostream&)) {
            if (cout.state) {
                cout.stream << manip;
            }
            if (file.state && file.stream.is_open()) {
                file.stream << manip;
            }
            return *this;
        }

        Log& operator<<(const LogDirection &dir);
        Log& to(Stream s);
};
#endif /* !LOG_H*/
