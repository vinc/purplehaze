#include <vector>
#include <algorithm>

#include "../../src/game.h"
#include "gtest/gtest.h"

class OrderingTest : public testing::Test
{
protected:
    Game game;
    std::string fen;

    virtual void SetUp() {
        // New game
        game.tt.clear();
        game.clear_killers();
        game.search_moves.clear();
    }
};

TEST_F(OrderingTest, Fine70Position)
{
    fen = "7b/1k4P1/2p1r2R/3P3K/8/8/8/8 w - - 0 1";

    const std::vector<Move> good_captures {
        Move(D5, E6, CAPTURE), // PxR
        Move(H6, E6, CAPTURE), // RxR
    };

    const std::vector<Move> promotion_captures {
        Move(G7, H8, QUEEN_PROMOTION_CAPTURE), // PxB
        Move(G7, H8, ROOK_PROMOTION_CAPTURE), // PxB
        Move(G7, H8, BISHOP_PROMOTION_CAPTURE), // PxB
        Move(G7, H8, KNIGHT_PROMOTION_CAPTURE), // PxB
    };

    const std::vector<Move> bad_captures {
        Move(H6, H8, CAPTURE), // RxB
        Move(D5, C6, CAPTURE), // PxP
    };

    const std::vector<Move> promotions {
        Move(G7, G8, KNIGHT_PROMOTION),
        Move(G7, G8, BISHOP_PROMOTION),
        Move(G7, G8, ROOK_PROMOTION),
        Move(G7, G8, QUEEN_PROMOTION),
    };

    const std::vector<Move> quiet_moves {
        Move(D5, D6, QUIET_MOVE), // P
        Move(H6, H7, QUIET_MOVE), // R
        Move(H6, G6, QUIET_MOVE), // R
        Move(H6, F6, QUIET_MOVE), // R
        Move(H5, H4, QUIET_MOVE), // K
        Move(H5, G4, QUIET_MOVE), // K
        Move(H5, G5, QUIET_MOVE), // K
        Move(H5, G6, QUIET_MOVE), // K
    };

    game.fen(fen);
    Moves moves(game.board, game.pieces, game.positions.current(),
                game.search_moves);
    ExtendedMove em;
    for (int i = 0; !(em = moves.next()).is_null(); ++i) {

        Move m = static_cast<Move>(em);

        /*
        std::cout << "i := " << i
                  << ", score := " << static_cast<int>(em.value())
                  << ", m := " << game.output_move(m)
                  << std::endl;
        */

        int n = 0;
        if (i < (n += good_captures.size())) {
            EXPECT_EQ(good_captures[i], m);
        } else if (i < (n += promotion_captures.size())) {
            const std::vector<Move>& ms = promotion_captures;
            EXPECT_NE(ms.end(), std::find(ms.begin(), ms.end(), m));
        } else if (i < (n += bad_captures.size())) {
            EXPECT_EQ(bad_captures[i - n + bad_captures.size()], m);
        } else if (i < (n += promotions.size())) {
            const std::vector<Move>& ms = promotions;
            EXPECT_NE(ms.end(), std::find(ms.begin(), ms.end(), m));
        } else {
            EXPECT_EQ(quiet_moves[i - n], m);
        }
    }
}
