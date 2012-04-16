#include <algorithm>
#include <climits>

#include "../../src/move.h"
#include "../../src/board.h"
#include "gtest/gtest.h"

// Helper template returning the size of a one dimension array at compile time
template<typename T, size_t N>
constexpr size_t array_size(const T (&)[N])
{
    return N;
}

static const MoveType MOVES[] = {
    QUIET_MOVE,
    DOUBLE_PAWN_PUSH,
    KING_CASTLE,
    QUEEN_CASTLE,
    CAPTURE,
    EN_PASSANT,
    NULL_MOVE,
    KNIGHT_PROMOTION,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION,
    KNIGHT_PROMOTION_CAPTURE,
    BISHOP_PROMOTION_CAPTURE,
    ROOK_PROMOTION_CAPTURE,
    QUEEN_PROMOTION_CAPTURE
};

static const MoveType PROMOTIONS[] = {
    KNIGHT_PROMOTION,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION,
    KNIGHT_PROMOTION_CAPTURE,
    BISHOP_PROMOTION_CAPTURE,
    ROOK_PROMOTION_CAPTURE,
    QUEEN_PROMOTION_CAPTURE
};
static const int NB_PROMOTIONS = array_size(PROMOTIONS);

static const MoveType CAPTURES[] = {
    CAPTURE,
    EN_PASSANT,
    KNIGHT_PROMOTION_CAPTURE,
    BISHOP_PROMOTION_CAPTURE,
    ROOK_PROMOTION_CAPTURE,
    QUEEN_PROMOTION_CAPTURE
};
static const int NB_CAPTURES = array_size(CAPTURES);

TEST(MoveTest, Size)
{
    EXPECT_EQ(2, sizeof(uint16_t));
    EXPECT_EQ(sizeof(uint16_t), sizeof(Move));
}

TEST(MoveTest, Constructor)
{
    Move null_move;
    EXPECT_TRUE(null_move.is_null());
    EXPECT_TRUE(Move().is_null());

    for (const Square &o : SQUARES) {
        if (o == OUT) continue;
        for (const Square &d : SQUARES) {
            if (d == OUT || d == o) continue;
            for (const MoveType &t : MOVES) {
                Move m(o, d, t);
                EXPECT_EQ(o, m.orig());
                EXPECT_EQ(d, m.dest());
                EXPECT_EQ(t, m.type());

                EXPECT_EQ(Board::get_file(o), m.orig_file());
                EXPECT_EQ(Board::get_file(d), m.dest_file());
                EXPECT_EQ(Board::get_rank(o), m.orig_rank());
                EXPECT_EQ(Board::get_rank(d), m.dest_rank());

                /*
                // FIXME: Takes too long, too much moves
                for (const Square &o2 : SQUARES) {
                    if (o2 == OUT) continue;
                    for (const Square &d2 : SQUARES) {
                        if (d2 == OUT || d2 == o2) continue;
                        for (const MoveType &t2 : MOVES) {
                            Move m2(o2, d2, t2);
                            if (o == o2 && d == d2 && t == t2) {
                                EXPECT_EQ(m2, m);
                            } else {
                                EXPECT_NE(m2, m);
                            }
                        }
                    }
                }
                */
            }
        }
    }
}

TEST(MoveTest, Type)
{
    for (const MoveType &t : MOVES) {
        // Define move types
        bool is_null = t == NULL_MOVE;
        bool is_en_passant = t == EN_PASSANT;
        bool is_double_pawn_push = t == DOUBLE_PAWN_PUSH;
        bool is_castle = t == KING_CASTLE || t == QUEEN_CASTLE;
        bool is_capture =
            std::binary_search(CAPTURES, CAPTURES + NB_CAPTURES, t);
        bool is_promotion =
            std::binary_search(PROMOTIONS, PROMOTIONS + NB_PROMOTIONS, t);

        for (const Square &o : SQUARES) {
            if (o == OUT) continue;
            for (const Square &d : SQUARES) {
                if (d == OUT || d == o) continue;
                Move m(o, d, t);
                EXPECT_EQ(is_null, m.is_null());
                EXPECT_EQ(is_en_passant, m.is_en_passant());
                EXPECT_EQ(is_double_pawn_push, m.is_double_pawn_push());
                EXPECT_EQ(is_castle, m.is_castle());
                EXPECT_EQ(is_capture, m.is_capture());
                EXPECT_EQ(is_promotion, m.is_promotion());

                switch (t) {
                    case KING_CASTLE:
                        EXPECT_EQ(KING, m.castle_side());
                        break;
                    case QUEEN_CASTLE:
                        EXPECT_EQ(QUEEN, m.castle_side());
                        break;
                    case KNIGHT_PROMOTION:
                    case KNIGHT_PROMOTION_CAPTURE:
                        EXPECT_EQ(KNIGHT, m.promotion_type());
                        break;
                    case BISHOP_PROMOTION:
                    case BISHOP_PROMOTION_CAPTURE:
                        EXPECT_EQ(BISHOP, m.promotion_type());
                        break;
                    case ROOK_PROMOTION:
                    case ROOK_PROMOTION_CAPTURE:
                        EXPECT_EQ(ROOK, m.promotion_type());
                        break;
                    case QUEEN_PROMOTION:
                    case QUEEN_PROMOTION_CAPTURE:
                        EXPECT_EQ(QUEEN, m.promotion_type());
                        break;
                    default:
                        break;
                }
            }
        }
    }
}


TEST(ExtendedMoveTest, Size)
{
    int size = sizeof(uint16_t) + sizeof(char) + 1; // 1 byte of padding
    EXPECT_EQ(4, size);
    EXPECT_EQ(size, sizeof(ExtendedMove));
}

TEST(ExtendedMoveTest, Constructor)
{
    EXPECT_EQ(Move(), ExtendedMove());
    EXPECT_EQ(0, ExtendedMove().value());

    for (const Square &o : SQUARES) {
        if (o == OUT) continue;
        for (const Square &d : SQUARES) {
            if (d == OUT || d == o) continue;
            for (const MoveType &t : MOVES) {
                Move m(o, d, t);
                ExtendedMove em1(m);
                ExtendedMove em2(m, 0);
                EXPECT_EQ(m, em1);
                EXPECT_EQ(em1, em2);

                // '==' and '!=' operators are inherited from Move class
                ExtendedMove em3(m, 50);
                EXPECT_EQ(em2, em3);
            }
        }
    }
}

TEST(ExtendedMoveTest, Score)
{
    Move m;
    for (int i = SCHAR_MIN; i <= SCHAR_MAX; ++i) {
        ExtendedMove em1(m, i);
        EXPECT_EQ(i, em1.value());

        // Test getter and setter
        ExtendedMove em2(m, 0);
        EXPECT_EQ(0, em2.value());
        em2.set_score(i);
        EXPECT_EQ(i, em2.value());
        em2.set_score(0);
        EXPECT_EQ(0, em2.value());
    }
}

TEST(ExtendedMoveTest, Comp)
{
    Move m;
    ExtendedMove em1(m, 0);
    ExtendedMove em2(m, SCHAR_MAX);
    for (int i = SCHAR_MIN; i <= SCHAR_MAX; ++i) {
        ExtendedMove em3(m, i);
        EXPECT_EQ(i < 0, em1 < em3);
        if (i < 0) {
            EXPECT_LT(em1, em3);
        }
        if (i < SCHAR_MAX) {
            EXPECT_LT(em2, em3);
        }
    }
}
