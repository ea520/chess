#include "chess.hpp"
bool in_board(int8_t x, int8_t y)
{
    return x <= 8 && x >= 1 && y <= 8 && y >= 1;
}

std::vector<coordinate_t> filter_check(const game_t &game, const std::vector<coordinate_t> &moves, bool white)
{
    std::vector<coordinate_t> ret;
    for (auto move : moves)
    {
        game_t g = game;
        g.move(move.x, move.y);
        if (!g.in_check(white))
            ret.push_back(move);
    }
    return ret;
}

std::vector<coordinate_t> piece_t::pawn_available_moves(const game_t &game, bool white, coordinate_t enpassant) const
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

std::vector<coordinate_t> piece_t::knight_available_moves(const game_t &game, bool white, coordinate_t enpassant) const
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

std::vector<coordinate_t> piece_t::bishop_available_moves(const game_t &game, bool white, coordinate_t enpassant) const
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
                bool is_black = game.get_black(newx, newy);
                bool is_white = game.get_white(newx, newy);
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

std::vector<coordinate_t> piece_t::rook_available_moves(const game_t &game, bool white, coordinate_t enpassant) const
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
                bool is_black = game.get_black(newx, newy);
                bool is_white = game.get_white(newx, newy);
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

std::vector<coordinate_t> piece_t::queen_available_moves(const game_t &game, bool white, coordinate_t enpassant) const
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
                bool is_black = game.get_black(newx, newy);
                bool is_white = game.get_white(newx, newy);
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

std::vector<coordinate_t> piece_t::king_available_moves(const game_t &game, bool white, coordinate_t enpassant) const
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
            bool is_black = game.get_black(newx, newy);
            bool is_white = game.get_white(newx, newy);
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
            piece_t castle_candidate;
            if (white)
                castle_candidate = game.get_white(rook_initial_x[i], y);
            else
                castle_candidate = game.get_black(rook_initial_x[i], y);

            if (castle_candidate && castle_candidate.isrook() && !castle_candidate.has_moved)
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

void piece_t::set_position(game_t &g, uint8_t x, uint8_t y)
{
    position.x = x;
    position.y = y;
    has_moved = true;
    g.get(x, y) = *this;
}

std::vector<coordinate_t> piece_t::available_moves(const game_t &game, bool white, coordinate_t enpassant) const
{

    std::vector<coordinate_t> ret;
    if (isinvalid())
        return {};
    switch (this->type)
    {
    case piece_type::pawn:
        ret = pawn_available_moves(game, white, enpassant);
        break;
    case piece_type::rook:
        ret = rook_available_moves(game, white, enpassant);

        break;
    case piece_type::king:
        ret = king_available_moves(game, white, enpassant);

        break;
    case piece_type::queen:
        ret = queen_available_moves(game, white, enpassant);

        break;
    case piece_type::bishop:
        ret = bishop_available_moves(game, white, enpassant);

        break;
    case piece_type::knight:
        ret = knight_available_moves(game, white, enpassant);

        break;
    default:
        assert(false);
        return {};
    }
    return filter_check(game, ret, white);
}
game_t::game_t() : white_king({0, 0}), black_king({0, 0})
{
    for (auto &row : board)
        for (auto &piece : row)
            piece = piece_t();

    for (int i = 1; i <= 8; i++)
        board[i - 1][2 - 1] = piece_t(i, 2, true, piece_type::pawn);
    for (int i = 1; i <= 8; i++)
        board[i - 1][7 - 1] = piece_t(i, 7, false, piece_type::pawn);

    for (int i : {1, 8})
        board[i - 1][1 - 1] = piece_t(i, 1, true, piece_type::rook);
    for (int i : {1, 8})
        board[i - 1][8 - 1] = piece_t(i, 8, false, piece_type::rook);

    for (int i : {2, 7})
        board[i - 1][1 - 1] = piece_t(i, 1, true, piece_type::knight);
    for (int i : {2, 7})
        board[i - 1][8 - 1] = piece_t(i, 8, false, piece_type::knight);

    for (int i : {3, 6})
        board[i - 1][1 - 1] = piece_t(i, 1, true, piece_type::bishop);
    for (int i : {3, 6})
        board[i - 1][8 - 1] = piece_t(i, 8, false, piece_type::bishop);

    board[4 - 1][1 - 1] = piece_t(4, 1, true, piece_type::queen);
    board[4 - 1][8 - 1] = piece_t(4, 8, false, piece_type::queen);

    board[5 - 1][1 - 1] = piece_t(5, 1, true, piece_type::king);
    board[5 - 1][8 - 1] = piece_t(5, 8, false, piece_type::king);

    white_king = board[5 - 1][1 - 1].get_position();
    black_king = board[5 - 1][8 - 1].get_position();
}

piece_t game_t::get(uint8_t x, uint8_t y) const
{
    return in_board(x, y) ? board[x - 1][y - 1] : piece_t();
}

piece_t &game_t::get(uint8_t x, uint8_t y)
{
    assert(in_board(x, y));
    return board[x - 1][y - 1];
}

void game_t::draw()
{
    for (auto &row : board)
        for (auto &piece : row)
            piece.draw();
}

void game_t::move(uint8_t x, uint8_t y)
{
    if (enpassant != coordinate_t{0, 0})
    {
        if (x == enpassant.x)
            if (white_turn && y == enpassant.y + 1 || !white_turn && y == enpassant.y - 1)
                get(enpassant) = piece_t();
    }
    if (current_piece.ispawn() && ((current_piece.get_position().y == 2 && y == 4) || (current_piece.get_position().y == 7 && y == 5)))
    // pawn just moved 2 places. save it's position
    {
        enpassant = {x, y};
    }
    else
    {
        enpassant = {0, 0};
    }
    auto abs_diff = [](uint8_t a, uint8_t b) -> uint8_t
    {
        return a > b ? a - b : b - a;
    };
    if (current_piece.isking() && abs_diff(x, current_piece.get_position().x) > 1)
    {
        // castling
        if (x == 3)
        {
            piece_t &_rook = get(1, y);
            _rook.set_position(*this, 4, y);
        }
        if (x == 7)
        {
            piece_t &_rook = get(8, y);
            _rook.set_position(*this, 6, y);
        }
    }
    if (!current_piece.isinvalid())
        get(current_piece.get_position()) = piece_t();
    current_piece.set_position(*this, x, y);
    set_current_piece(current_piece);
}

bool game_t::in_check(bool white) const
{
    std::vector<coordinate_t> pawns, rooks, bishops, queens, knights, kings;
    auto &king_to_check = white ? white_king : black_king;
    for (const auto &row : board)
        for (const auto &piece : row)
            if (piece.iswhite() != white)
                switch (piece.get_type())
                {
                case piece_type::pawn:
                    pawns.push_back(piece.get_position());
                    break;

                case piece_type::rook:
                    rooks.push_back(piece.get_position());
                    break;
                case piece_type::king:
                    kings.push_back(piece.get_position());
                    break;

                case piece_type::bishop:
                    bishops.push_back(piece.get_position());
                    break;

                case piece_type::queen:
                    queens.push_back(piece.get_position());
                    break;

                case piece_type::knight:
                    knights.push_back(piece.get_position());
                    break;

                default:
                    assert(piece.get_type() == piece_type::invalid);
                    break;
                }

    auto abs_diff = [](uint8_t a, uint8_t b) -> uint8_t
    {
        return a > b ? a - b : b - a;
    };

    for (auto pawn : pawns)
    {
        if (white)
        {
            if (pawn.y == king_to_check.y + 1 && abs_diff(king_to_check.x, pawn.x) == 1)
                return true;
        }
        else
        {
            if (pawn.y == king_to_check.y - 1 && abs_diff(king_to_check.x, pawn.x) == 1)
                return true;
        }
    }

    for (auto knight : knights)
    {
        uint8_t dy = abs_diff(knight.y, king_to_check.y);
        uint8_t dx = abs_diff(knight.x, king_to_check.x);
        if ((dy == 1 && dx == 2) || (dy == 2 && dx == 1))
            return true;
    }

    for (auto king : kings)
    {
        uint8_t dy = abs_diff(king.y, king_to_check.y);
        uint8_t dx = abs_diff(king.x, king_to_check.x);
        if ((dy == 0 || dy == 1) && (dx == 0 && dx == 1) && (dx + dy != 0))
            return true;
    }

    for (auto rook : rooks)
    {
        if (king_to_check.x == rook.x)
        {
            uint8_t lower = std::min(king_to_check.y, rook.y);
            uint8_t upper = std::max(king_to_check.y, rook.y);
            bool b = true;
            for (uint8_t i = lower + 1; i < upper; i++)
            {
                b = b && !get(rook.x, i);
            }
            if (b)
                return true;
        }
        if (king_to_check.y == rook.y)
        {
            uint8_t lower = std::min(king_to_check.x, rook.x);
            uint8_t upper = std::max(king_to_check.x, rook.x);
            bool b = true;
            for (uint8_t i = lower + 1; i < upper; i++)
            {
                b = b && !get(i, rook.y);
            }
            if (b)
                return true;
        }
    }
    for (auto bishop : bishops)
    {
        uint8_t dy = abs_diff(bishop.y, king_to_check.y);
        uint8_t dx = abs_diff(bishop.x, king_to_check.x);
        if (dx == dy)
        {
            int xdirection = bishop.x < king_to_check.x ? 1 : -1;
            int ydirection = bishop.y < king_to_check.y ? 1 : -1;
            uint8_t x = bishop.x + xdirection, y = bishop.y + ydirection;
            bool b = true;
            for (uint8_t i = 1; i < dx; i++)
            {
                b = b && !get(x, y);
                x += xdirection;
                y += ydirection;
            }
            if (b)
                return true;
        }
    }
    for (auto queen : queens)
    {
        if (king_to_check.x == queen.x)
        {
            uint8_t lower = std::min(king_to_check.y, queen.y);
            uint8_t upper = std::max(king_to_check.y, queen.y);
            bool b = true;
            for (uint8_t i = lower + 1; i < upper; i++)
            {
                b = b && !get(queen.x, i);
            }
            if (b)
                return true;
        }
        if (king_to_check.y == queen.y)
        {
            uint8_t lower = std::min(king_to_check.x, queen.x);
            uint8_t upper = std::max(king_to_check.x, queen.x);
            bool b = true;
            for (uint8_t i = lower + 1; i < upper; i++)
            {
                b = b && !get(i, queen.y);
            }
            if (b)
                return true;
        }

        uint8_t dy = abs_diff(queen.y, king_to_check.y);
        uint8_t dx = abs_diff(queen.x, king_to_check.x);
        if (dx == dy)
        {
            int xdirection = queen.x < king_to_check.x ? 1 : -1;
            int ydirection = queen.y < king_to_check.y ? 1 : -1;
            uint8_t x = queen.x + xdirection, y = queen.y + ydirection;
            bool b = true;
            for (uint8_t i = 1; i < dx; i++)
            {
                b = b && !get(x, y);
                x += xdirection;
                y += ydirection;
            }
            if (b)
                return true;
        }
    }
    return false;
}
bool game_t::in_check_mate(bool white) const
{
    if (!in_check(white))
        return false;
    std::vector<piece_t> pieces;
    if (white)
        pieces = get_white_pieces();
    else
        pieces = get_black_pieces();

    for (const auto &piece : pieces)
    {
        game_t g = *this;
        assert(g.in_check(white));
        g.set_current_piece(piece);
        auto moves = piece.available_moves(g, white, enpassant);
        if (moves.size())
            return false;
    }
    return true;
}