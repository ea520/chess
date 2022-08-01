#include "chess.hpp"
bool in_board(int8_t x, int8_t y)
{
    return x <= 8 && x >= 1 && y <= 8 && y >= 1;
}

std::vector<coordinate_t> pawn::available_moves(const game_t &game, bool white, coordinate_t enpassant) const
{
    std::vector<coordinate_t> ret;
    if (white)
    {
        // 1 forward in general
        if (position.y < 8 && !game.get(position.x, position.y + 1))
        {
            ret.emplace_back(coordinate_t(position.x, position.y + 1));

            // 2 forward at start check here to avoid jumping over pieces
            if (position.y == 2 && !game.get(position.x, position.y + 2))
            {
                ret.emplace_back(coordinate_t(position.x, position.y + 2));
            }
        }
        piece_t *candidate = game.get_black(position.x + 1, position.y + 1);
        // take forward and right
        if (position.y < 8 && position.x < 8 && candidate && !candidate->isking())
        {
            ret.emplace_back(coordinate_t(position.x + 1, position.y + 1));
        }
        candidate = game.get_black(position.x - 1, position.y + 1);
        // take forward and left
        if (position.y < 8 && position.x > 1 && candidate && !candidate->isking())
        {
            ret.emplace_back(coordinate_t(position.x - 1, position.y + 1));
        }
    }
    else
    {
        // 1 forward in general
        if (position.y > 1 && !game.get(position.x, position.y - 1))
        {
            ret.emplace_back(coordinate_t(position.x, position.y - 1));

            // 2 forward at start check here to avoid jumping over pieces
            if (position.y == 7 && !game.get(position.x, position.y - 2))
            {
                ret.emplace_back(coordinate_t(position.x, position.y - 2));
            }
        }

        // take forward and right
        piece_t *candidate = game.get_white(position.x + 1, position.y - 1);
        if (position.y > 1 && position.x < 8 && candidate && !candidate->isking())
        {
            ret.emplace_back(coordinate_t(position.x + 1, position.y - 1));
        }
        candidate = game.get_white(position.x - 1, position.y - 1);
        // take forward and left
        if (position.y > 1 && position.x > 1 && candidate && !candidate->isking())
        {
            ret.emplace_back(coordinate_t(position.x - 1, position.y - 1));
        }
    }

    auto abs_diff = [](uint8_t a, uint8_t b) -> uint8_t
    {
        return a > b ? a - b : b - a;
    };
    if (enpassant.y == position.y && abs_diff(position.x, enpassant.x) == 1)
    {
        if (white)
            ret.emplace_back(coordinate_t(enpassant.x, enpassant.y + 1));
        else
            ret.emplace_back(coordinate_t(enpassant.x, enpassant.y - 1));
    }
    return ret;
}

std::vector<coordinate_t> knight::available_moves(const game_t &game, bool white, coordinate_t enpassant) const
{
    const static std::array<std::array<int8_t, 2>, 8> knight_displacements = {
        std::array<int8_t, 2>{+1, +2},
        std::array<int8_t, 2>{+1, -2},
        std::array<int8_t, 2>{-1, +2},
        std::array<int8_t, 2>{-1, -2},
        std::array<int8_t, 2>{+2, +1},
        std::array<int8_t, 2>{+2, -1},
        std::array<int8_t, 2>{-2, +1},
        std::array<int8_t, 2>{-2, -1}};
    std::vector<coordinate_t> ret;
    for (std::array<int8_t, 2> displacement : knight_displacements)
    {
        int8_t newx = position.x + displacement[0];
        int8_t newy = position.y + displacement[1];
        if (in_board(newx, newy))
        {
            if (white && !game.get_white(newx, newy))
            {
                piece_t *potential_king = game.get_black(newx, newy);
                if (!potential_king || !potential_king->isking())
                    ret.emplace_back(newx, newy);
            }
            else if (!white && !game.get_black(newx, newy))
            {
                piece_t *potential_king = game.get_white(newx, newy);
                if (!potential_king || !potential_king->isking())
                    ret.emplace_back(newx, newy);
            }
        }
    }
    return ret;
}

std::vector<coordinate_t> bishop::available_moves(const game_t &game, bool white, coordinate_t enpassant) const
{
    std::vector<coordinate_t> ret;

    static constexpr std::array<std::array<int8_t, 2>, 4> bishop_displacements =
        {
            std::array<int8_t, 2>{1, 1},
            std::array<int8_t, 2>{1, -1},
            std::array<int8_t, 2>{-1, -1},
            std::array<int8_t, 2>{-1, 1}};
    for (auto increment : bishop_displacements)
    {
        int newx = position.x, newy = position.y;
        while (true)
        {
            newx += increment[0];
            newy += increment[1];
            if (in_board(newx, newy))
            {
                piece_t *black_piece_to_take = game.get_black(newx, newy);
                piece_t *white_piece_to_take = game.get_white(newx, newy);
                bool is_black = black_piece_to_take != nullptr;
                bool is_white = white_piece_to_take != nullptr;
                bool is_free = !is_black && !is_white;
                if (is_free)
                    ret.emplace_back(newx, newy);
                else if (white && is_black && !black_piece_to_take->isking())
                {
                    // you can take the piece
                    ret.emplace_back(newx, newy);
                    break;
                }
                else if (white && is_white)
                {
                    break;
                }
                else if (!white && is_black)
                {
                    break;
                }
                else if (!white && is_white && !white_piece_to_take->isking())
                {
                    ret.emplace_back(newx, newy);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    return ret;
}

std::vector<coordinate_t> rook::available_moves(const game_t &game, bool white, coordinate_t enpassant) const
{
    std::vector<coordinate_t> ret;

    static constexpr std::array<std::array<int8_t, 2>, 4> bishop_displacements =
        {
            std::array<int8_t, 2>{+1, +0},
            std::array<int8_t, 2>{-1, +0},
            std::array<int8_t, 2>{+0, +1},
            std::array<int8_t, 2>{+0, -1}};
    for (auto increment : bishop_displacements)
    {
        int newx = position.x, newy = position.y;
        while (true)
        {
            newx += increment[0];
            newy += increment[1];
            if (in_board(newx, newy))
            {
                piece_t *black_piece_to_take = game.get_black(newx, newy);
                piece_t *white_piece_to_take = game.get_white(newx, newy);
                bool is_black = black_piece_to_take != nullptr;
                bool is_white = white_piece_to_take != nullptr;
                bool is_free = !is_black && !is_white;
                if (is_free)
                    ret.emplace_back(newx, newy);
                else if (white && is_black && !black_piece_to_take->isking())
                {
                    // you can take the piece
                    ret.emplace_back(newx, newy);
                    break;
                }
                else if (white && is_white)
                {
                    break;
                }
                else if (!white && is_black)
                {
                    break;
                }
                else if (!white && is_white && !white_piece_to_take->isking())
                {
                    ret.emplace_back(newx, newy);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    return ret;
}

std::vector<coordinate_t> queen::available_moves(const game_t &game, bool white, coordinate_t enpassant) const
{
    std::vector<coordinate_t> ret;

    static constexpr std::array<std::array<int8_t, 2>, 8> bishop_displacements =
        {
            std::array<int8_t, 2>{+1, +0},
            std::array<int8_t, 2>{-1, +0},
            std::array<int8_t, 2>{+0, +1},
            std::array<int8_t, 2>{+0, -1},
            std::array<int8_t, 2>{1, 1},
            std::array<int8_t, 2>{1, -1},
            std::array<int8_t, 2>{-1, -1},
            std::array<int8_t, 2>{-1, 1}};
    for (auto increment : bishop_displacements)
    {
        int newx = position.x, newy = position.y;
        while (true)
        {
            newx += increment[0];
            newy += increment[1];
            if (in_board(newx, newy))
            {
                piece_t *black_piece_to_take = game.get_black(newx, newy);
                piece_t *white_piece_to_take = game.get_white(newx, newy);
                bool is_black = black_piece_to_take != nullptr;
                bool is_white = white_piece_to_take != nullptr;
                bool is_free = !is_black && !is_white;
                if (is_free)
                    ret.emplace_back(newx, newy);
                else if (white && is_black && !black_piece_to_take->isking())
                {
                    // you can take the piece
                    ret.emplace_back(newx, newy);
                    break;
                }
                else if (white && is_white)
                {
                    break;
                }
                else if (!white && is_black)
                {
                    break;
                }
                else if (!white && is_white && !white_piece_to_take->isking())
                {
                    ret.emplace_back(newx, newy);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    return ret;
}

std::vector<coordinate_t> king::available_moves(const game_t &game, bool white, coordinate_t enpassant) const
{
    std::vector<coordinate_t> ret;

    static constexpr std::array<std::array<int8_t, 2>, 8> bishop_displacements =
        {
            std::array<int8_t, 2>{+1, +0},
            std::array<int8_t, 2>{-1, +0},
            std::array<int8_t, 2>{+0, +1},
            std::array<int8_t, 2>{+0, -1},
            std::array<int8_t, 2>{1, 1},
            std::array<int8_t, 2>{1, -1},
            std::array<int8_t, 2>{-1, -1},
            std::array<int8_t, 2>{-1, 1}};
    for (auto increment : bishop_displacements)
    {
        int newx = position.x, newy = position.y;
        newx += increment[0];
        newy += increment[1];
        if (in_board(newx, newy))
        {
            // needn't check that a king isn't taking another king
            bool is_black = game.get_black(newx, newy) != nullptr;
            bool is_white = game.get_white(newx, newy) != nullptr;
            bool is_free = !is_black && !is_white;
            if (is_free)
                ret.emplace_back(newx, newy);
            else if (white && is_black)
            {
                // you can take the piece
                ret.emplace_back(newx, newy);
            }

            else if (!white && is_white) // black piece potentially taking a white piece
            {
                ret.emplace_back(newx, newy);
            }
        }
    }

    if (!has_moved)
    {
        uint8_t y = white ? 1 : 8;
        // rooks start on column 1 or 8
        uint8_t rook_initial_x[2] = {1, 8};

        // for castling left, column [2, 5) must be empty
        // for castling right, column [6, 8) must be empty
        uint8_t intermediate_bounds[2][2] = {{2, 5}, {6, 8}};
        for (unsigned i = 0; i < 2; i++)
        {
            const piece_t *castle_candidate = nullptr;
            if (white)
                castle_candidate = game.get_white(rook_initial_x[i], y);
            else
                castle_candidate = game.get_black(rook_initial_x[i], y);

            if (castle_candidate && castle_candidate->isrook() && !castle_candidate->has_moved)
            {
                bool all_empty = true;
                uint8_t lower = intermediate_bounds[i][0];
                uint8_t upper = intermediate_bounds[i][1];
                for (int i = lower; i < upper; i++)
                {
                    all_empty = all_empty && !game.get(i, y);
                }
                if (all_empty)
                    ret.emplace_back(lower + 1, y);
            }
        }
    }

    return ret;
}