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

#ifndef COMMON_H
#define COMMON_H

#include <string>

#ifndef VERSION
#define VERSION "2.1.0"
#endif

static const int BOARD_SIZE  = 128;
static const int MAX_PLY     = 128; // Maximum search depth
static const int MAX_MOVES   = 256; // Maximum number of moves per position
static const int MAX_KILLERS = 2;
static const int INF         = 29999;
static const int TT_SIZE     = 128 << 20; // 128 MB
static const int MT_SIZE     = 1 << 20; // 1 MB

static const std::string DEFAULT_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

enum Square : uint8_t {
    A1=0x00, B1, C1, D1, E1, F1, G1, H1,
    A2=0x10, B2, C2, D2, E2, F2, G2, H2,
    A3=0x20, B3, C3, D3, E3, F3, G3, H3,
    A4=0x30, B4, C4, D4, E4, F4, G4, H4,
    A5=0x40, B5, C5, D5, E5, F5, G5, H5,
    A6=0x50, B6, C6, D6, E6, F6, G6, H6,
    A7=0x60, B7, C7, D7, E7, F7, G7, H7,
    A8=0x70, B8, C8, D8, E8, F8, G8, H8,
    OUT
};

static const Square SQUARES[] = {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    OUT
};

enum File : uint8_t {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H
};

enum Rank : uint8_t {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8
};

enum Direction {
    NO_DIR           = 0,
    UP               = 0x10,
    RIGHT            = 0x01,
    DOWN             = -UP,
    LEFT             = -RIGHT,
    UP_RIGHT         = UP + RIGHT,
    DOWN_RIGHT       = DOWN + RIGHT,
    DOWN_LEFT        = DOWN + LEFT,
    UP_LEFT          = UP + LEFT,
    UP_UP_RIGHT      = UP + UP_RIGHT,
    RIGHT_UP_RIGHT   = RIGHT + UP_RIGHT,
    RIGHT_DOWN_RIGHT = RIGHT + DOWN_RIGHT,
    DOWN_DOWN_RIGHT  = DOWN + DOWN_RIGHT,
    DOWN_DOWN_LEFT   = DOWN + DOWN_LEFT,
    LEFT_DOWN_LEFT   = LEFT + DOWN_LEFT,
    LEFT_UP_LEFT     = LEFT + UP_LEFT,
    UP_UP_LEFT       = UP + UP_LEFT
};

#ifdef __INTEL_COMPILER
enum Color : uint8_t {
#else
enum Color : bool {
#endif
    WHITE,
    BLACK
};
static const Color COLORS[] = { WHITE, BLACK };
inline Color operator!(Color c)
{
    return static_cast<Color>(!static_cast<bool>(c));
}

enum PieceType : uint8_t {
    EMPTY,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};
static const int NB_PIECE_TYPES = static_cast<int>(KING) + 1;

static const PieceType PIECE_TYPES[] = {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};
static const PieceType NOT_PAWN_TYPES[] = {
    KNIGHT, BISHOP, ROOK, QUEEN, KING
};
static const PieceType SIDE_TYPES[] = {
    QUEEN, KING
};
static const PieceType MINOR_TYPES[] = {
    KNIGHT, BISHOP
};
static const PieceType MAJOR_TYPES[] = {
    ROOK, QUEEN
};

enum MoveType : uint8_t {
    QUIET_MOVE,
    DOUBLE_PAWN_PUSH,
    KING_CASTLE,
    QUEEN_CASTLE,
    CAPTURE,
    EN_PASSANT,
    NULL_MOVE,            // Warning: is_capture() will return true
    KNIGHT_PROMOTION = 8, // is_promotion() { MoveType[3] }
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION,
    KNIGHT_PROMOTION_CAPTURE,
    BISHOP_PROMOTION_CAPTURE,
    ROOK_PROMOTION_CAPTURE,
    QUEEN_PROMOTION_CAPTURE
};

// Used in movegen.cpp and attack.cpp
static const int NB_DIRS[] = { 0, 0, 8, 4, 4, 8, 8 };
static const Direction PIECES_DIRS[][8] = {
    {
        // Empty
        NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR
    },
    {
        // Pawns
        NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR
    },
    {
        // Knights
        UP_UP_RIGHT, RIGHT_UP_RIGHT, RIGHT_DOWN_RIGHT, DOWN_DOWN_RIGHT,
        DOWN_DOWN_LEFT, LEFT_DOWN_LEFT, LEFT_UP_LEFT, UP_UP_LEFT
    },
    {
        // Bishops
        UP_RIGHT, DOWN_RIGHT, DOWN_LEFT, UP_LEFT,
        NO_DIR, NO_DIR, NO_DIR, NO_DIR
    },
    {
        // Rooks
        UP, RIGHT, DOWN, LEFT, NO_DIR, NO_DIR, NO_DIR, NO_DIR
    },
    {
        // Queens
        UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT
    },
    {
        // Kings
        UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT
    }
};

// Used in movegen.cpp and protocol.cpp
static const Direction PAWN_PUSH_DIRS[] = { UP, DOWN };
static const Direction PAWN_CAPTURE_DIRS[][2] = {
    {
        UP_LEFT, UP_RIGHT
    },
    {
        DOWN_LEFT, DOWN_RIGHT
    }
};

// Used in tt.h
enum Bound : uint8_t { EXACT, LOWER, UPPER, UNDEF_BOUND };

// Used in game.h, hashtable.cpp
// NOTE: sizeof(Material) + sizeof(Hash) must be a power of two
// for efficient material hashtable lookup.
typedef uint64_t Material;

// Used for debugging
#define assert_msg(x) !(std::cerr << "Assertion failed: " << x << std::endl)

/*
// Overload operators to solve ambiguous errors with Clang < 3.1
// TODO Find out if this could be better done
inline bool operator==(int i, Rank r)
{
    return i == static_cast<int>(r);
}
inline bool operator<(Bound b, int i)
{
    return static_cast<char>(b) < i;
}
inline int operator>>(Square s, int i)
{
    return static_cast<int>(s) >> i;
}
inline int operator|(int i, MoveType mt)
{
    return i | static_cast<int>(mt);
}
inline int operator&(Square s, int i)
{
    return static_cast<int>(s) & i;
}
inline int operator*(Rank r, Color c)
{
    return static_cast<int>(r) * static_cast<int>(c);
}
inline int operator*(int i, Rank r)
{
    return i * static_cast<int>(r);
}
inline int operator*(int i, Color c)
{
    return i * static_cast<int>(c);
}
inline int operator*(int i, PieceType pt)
{
    return i * static_cast<int>(pt);
}
inline int operator+(int i, File f)
{
    return i + static_cast<int>(f);
}
inline int operator+(int i, Square s)
{
    return i + static_cast<int>(s);
}
inline int operator+(int i, PieceType pt)
{
    return i + static_cast<int>(pt);
}
inline int operator+(PieceType pt, int i)
{
    return i + static_cast<int>(pt);
}
inline int operator+(Square s, int i)
{
    return i + static_cast<int>(s);
}
inline int operator+(Square a, Square b)
{
    return static_cast<int>(a) + static_cast<int>(b);
}
inline int operator+(Rank r, int i)
{
    return static_cast<int>(r) + i;
}
inline int operator+(char c, Rank r)
{
    return c + static_cast<char>(r);
}
inline int operator+(Square s, Direction d)
{
    return static_cast<int>(s) + static_cast<int>(d);
}
inline int operator-(Square a, Square b)
{
    return static_cast<int>(a) - static_cast<int>(b);
}
inline int operator-(Square s, int i)
{
    return static_cast<int>(s) - i;
}
inline int operator-(int i, Square s)
{
    return i - static_cast<int>(s);
}
inline int operator-(int i, PieceType pt)
{
    return i - static_cast<int>(pt);
}
inline int operator-(Rank a, Rank b)
{
    return static_cast<int>(a) - static_cast<int>(b);
}
inline int operator-(Rank r, int i)
{
    return static_cast<int>(r) - i;
}
inline int operator+(MoveType a, MoveType b)
{
    return static_cast<MoveType>(static_cast<int>(a) + static_cast<int>(b));
}
*/

// ANSI Color
static const std::string RED = "\x1b[31m";
static const std::string GREEN = "\x1b[32m";
static const std::string color_reset = "\x1b[0m";

#endif /* !COMMON_H */
