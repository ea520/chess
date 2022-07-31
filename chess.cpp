#include "chess.h"
bool in_board(int8_t x, int8_t y)
{
    return x <= 8 && x >= 1 && y <= 8 && y >= 1;
}

std::vector<coordinate_t> pawn::available_moves(const game_t &game, bool white) const
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

        // take forward and right
        if (position.y < 8 && position.x < 8 && game.get_black(position.x + 1, position.y + 1))
        {
            ret.emplace_back(coordinate_t(position.x + 1, position.y + 1));
        }
        // take forward and left
        if (position.y < 8 && position.x > 1 && game.get_black(position.x - 1, position.y + 1))
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
        if (position.y > 1 && position.x < 8 && game.get_white(position.x + 1, position.y - 1))
        {
            ret.emplace_back(coordinate_t(position.x + 1, position.y - 1));
        }
        // take forward and left
        if (position.y > 1 && position.x > 1 && game.get_white(position.x - 1, position.y - 1))
        {
            ret.emplace_back(coordinate_t(position.x - 1, position.y - 1));
        }
    }
    return ret;
}

std::vector<coordinate_t> knight::available_moves(const game_t &game, bool white) const
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
                ret.emplace_back(newx, newy);
            else if (!white && !game.get_black(newx, newy))
                ret.emplace_back(newx, newy);
        }
    }
    return ret;
}

std::vector<coordinate_t> bishop::available_moves(const game_t &game, bool white) const
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
                bool is_black = game.get_black(newx, newy) != nullptr;
                bool is_white = game.get_white(newx, newy) != nullptr;
                bool is_free = !is_black && !is_white;
                if (is_free)
                    ret.emplace_back(newx, newy);
                else if (white && is_black)
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
                else // black piece potentially taking a white piece
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

std::vector<coordinate_t> rook::available_moves(const game_t &game, bool white) const
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
                bool is_black = game.get_black(newx, newy) != nullptr;
                bool is_white = game.get_white(newx, newy) != nullptr;
                bool is_free = !is_black && !is_white;
                if (is_free)
                    ret.emplace_back(newx, newy);
                else if (white && is_black)
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
                else // black piece potentially taking a white piece
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

std::vector<coordinate_t> queen::available_moves(const game_t &game, bool white) const
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
                bool is_black = game.get_black(newx, newy) != nullptr;
                bool is_white = game.get_white(newx, newy) != nullptr;
                bool is_free = !is_black && !is_white;
                if (is_free)
                    ret.emplace_back(newx, newy);
                else if (white && is_black)
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
                else // black piece potentially taking a white piece
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

std::vector<coordinate_t> king::available_moves(const game_t &game, bool white) const
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

    return ret;
}