#pragma once
#include <vector>
#include <cassert>
#include <cstdio>
#include <memory>
#include <algorithm>
#include "rendering.hpp"
#include <stb_image.hpp>
struct coordinate_t
{
    coordinate_t(uint8_t x, uint8_t y) : x(x), y(y)
    {
    }
    uint8_t x : 4;
    uint8_t y : 4;
    bool operator==(coordinate_t right) const
    {
        return x == right.x && y == right.y;
    }
};
struct game_t;
struct piece_t
{
    piece_t(uint8_t x, uint8_t y, bool white) : position({x, y})
    {
    }
    coordinate_t position;
    bool has_moved = false;
    drawing_params params;
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white, coordinate_t enpassant) const = 0;
    virtual void draw() const
    {
        params.draw(position.x, position.y);
    };
    inline virtual bool ispawn() const { return false; }
    inline virtual bool isrook() const { return false; }
    inline virtual bool isking() const { return false; }
};
struct pawn : public piece_t
{
    pawn(uint8_t x, uint8_t y, bool white) : piece_t(x, y, white)
    {
        stbi_set_flip_vertically_on_load(!white);
        int w, h;
        const char *filename = white ? R"(white-pawn.png)" : R"(black-pawn.png)";
        auto data = stbi_load(filename, &w, &h, nullptr, 4);
        params = setup_square(data, w, h, 0.25f);
        stbi_image_free(data);
        position = {x, y};
    }
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white, coordinate_t enpassant) const override;
    inline virtual bool ispawn() const override { return true; }
};
struct bishop : public piece_t
{
    bishop(uint8_t x, uint8_t y, bool white) : piece_t(x, y, white)
    {
        stbi_set_flip_vertically_on_load(!white);
        int w, h;
        const char *filename = white ? R"(white-bishop.png)" : R"(black-bishop.png)";
        auto data = stbi_load(filename, &w, &h, nullptr, 4);
        params = setup_square(data, w, h, 0.25f);
        stbi_image_free(data);
        position = {x, y};
    }
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white, coordinate_t enpassant) const override;
};

struct rook : public piece_t
{
    rook(uint8_t x, uint8_t y, bool white) : piece_t(x, y, white)
    {
        stbi_set_flip_vertically_on_load(!white);
        int w, h;
        const char *filename = white ? R"(white-rook.png)" : R"(black-rook.png)";
        auto data = stbi_load(filename, &w, &h, nullptr, 4);
        params = setup_square(data, w, h, 0.25f);
        stbi_image_free(data);
        position = {x, y};
    }
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white, coordinate_t enpassant) const override;
    inline virtual bool isrook() const override { return true; }
};

struct knight : public piece_t
{
    knight(uint8_t x, uint8_t y, bool white) : piece_t(x, y, white)
    {
        stbi_set_flip_vertically_on_load(!white);
        int w, h;
        const char *filename = white ? R"(white-knight.png)" : R"(black-knight.png)";
        auto data = stbi_load(filename, &w, &h, nullptr, 4);
        params = setup_square(data, w, h, 0.25f);
        stbi_image_free(data);
        position = {x, y};
    }
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white, coordinate_t enpassant) const override;
};

struct king : public piece_t
{
    king(uint8_t x, uint8_t y, bool white) : piece_t(x, y, white)
    {
        stbi_set_flip_vertically_on_load(!white);
        int w, h;
        const char *filename = white ? R"(white-king.png)" : R"(black-king.png)";
        auto data = stbi_load(filename, &w, &h, nullptr, 4);
        params = setup_square(data, w, h, 0.25f);
        stbi_image_free(data);
        position = {x, y};
    }
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white, coordinate_t enpassant) const override;
    inline virtual bool isking() const { return true; }
};

struct queen : public piece_t
{
    queen(uint8_t x, uint8_t y, bool white) : piece_t(x, y, white)
    {
        stbi_set_flip_vertically_on_load(!white);
        int w, h;
        const char *filename = white ? R"(white-queen.png)" : R"(black-queen.png)";
        auto data = stbi_load(filename, &w, &h, nullptr, 4);
        params = setup_square(data, w, h, 0.25f);
        stbi_image_free(data);
        position = {x, y};
    }
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white, coordinate_t enpassant) const override;
};

struct game_t
{
    game_t()
    {
        for (int i = 1; i <= 8; i++)
            white_pieces.emplace_back(new pawn(i, 2, true));
        for (int i = 1; i <= 8; i++)
            black_pieces.emplace_back(new pawn(i, 7, false));

        for (int i : {1, 8})
            white_pieces.emplace_back(new rook(i, 1, true));
        for (int i : {1, 8})
            black_pieces.emplace_back(new rook(i, 8, false));

        for (int i : {2, 7})
            white_pieces.emplace_back(new knight(i, 1, true));
        for (int i : {2, 7})
            black_pieces.emplace_back(new knight(i, 8, false));

        for (int i : {3, 6})
            white_pieces.emplace_back(new bishop(i, 1, true));
        for (int i : {3, 6})
            black_pieces.emplace_back(new bishop(i, 8, false));

        white_pieces.emplace_back(new queen(4, 1, true));
        black_pieces.emplace_back(new queen(4, 8, false));
        white_pieces.emplace_back(new king(5, 1, true));
        black_pieces.emplace_back(new king(5, 8, false));
    }
    // do not delete the piece
    piece_t *get(uint8_t x, uint8_t y) const
    {
        piece_t *piece = get_white(x, y);
        if (piece)
            return piece;

        piece = get_black(x, y);
        if (piece)
            return piece;
        return nullptr;
    }
    piece_t *get(uint8_t x, uint8_t y, bool white) const
    {
        return white ? get_white(x, y) : get_black(x, y);
    }
    piece_t *get_white(uint8_t x, uint8_t y) const
    {
        for (const auto &piece : white_pieces)
        {
            if (piece->position.x == x && piece->position.y == y)
                return piece.get();
        }
        return nullptr;
    }
    piece_t *get_black(uint8_t x, uint8_t y) const
    {
        for (const auto &piece : black_pieces)
        {
            if (piece->position.x == x && piece->position.y == y)
                return piece.get();
        }
        return nullptr;
    }

    void draw()
    {
        for (const auto &piece : white_pieces)
            piece->draw();
        for (const auto &piece : black_pieces)
            piece->draw();
    }
    void update_position(piece_t *piece, uint8_t x, uint8_t y)
    {
        // if there's a piece already there, remove it
        auto same_pos = [x, y](const std::unique_ptr<piece_t> &candidate)
        {
            return coordinate_t{x, y} == candidate->position;
        };
        if (white_turn)
            black_pieces.erase(std::remove_if(black_pieces.begin(), black_pieces.end(), same_pos), black_pieces.end());
        else
            white_pieces.erase(std::remove_if(white_pieces.begin(), white_pieces.end(), same_pos), white_pieces.end());

        if (!(enpassant == coordinate_t(0, 0)))
        {
            piece_t *to_remove = nullptr;
            auto same_piece = [&to_remove](const std::unique_ptr<piece_t> &candidate)
            {
                return to_remove == candidate.get();
            };

            if (piece->ispawn() && enpassant.x == x)
            {
                if (white_turn && y == enpassant.y + 1)
                {
                    to_remove = get(enpassant.x, enpassant.y);
                    assert(to_remove);
                    black_pieces.erase(std::remove_if(black_pieces.begin(), black_pieces.end(), same_piece), black_pieces.end());
                }
                else if (!white_turn && y == enpassant.y - 1)
                {
                    to_remove = get(enpassant.x, enpassant.y);
                    assert(to_remove);
                    white_pieces.erase(std::remove_if(white_pieces.begin(), white_pieces.end(), same_piece), white_pieces.end());
                }
            }
        }
        if (piece->ispawn() && ((piece->position.y == 2 && y == 4) || (piece->position.y == 7 && y == 5)))
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
        if (piece->isking() && abs_diff(x, piece->position.x) > 1)
        {
            // castling
            if (x == 3)
            {
                piece_t *_rook = get(1, y);
                _rook->position.x = 4;
            }
            if (x == 7)
            {
                piece_t *_rook = get(8, y);
                _rook->position.x = 6;
            }
        }
        piece->position.x = x;
        piece->position.y = y;
        piece->has_moved = true;
    }
    void delete_current_piece()
    {
        auto same_piece = [this](const std::unique_ptr<piece_t> &candidate)
        {
            return current_piece == candidate.get();
        };

        if (white_turn)
            white_pieces.erase(std::remove_if(white_pieces.begin(), white_pieces.end(), same_piece), white_pieces.end());
        else
            black_pieces.erase(std::remove_if(black_pieces.begin(), black_pieces.end(), same_piece), black_pieces.end());
        current_piece = nullptr;
    }
    std::vector<std::unique_ptr<piece_t>> white_pieces;
    std::vector<std::unique_ptr<piece_t>> black_pieces;
    coordinate_t enpassant{0, 0};
    std::vector<coordinate_t> moves;
    bool white_turn = true;
    piece_t *current_piece = nullptr;
    bool promote = false;
};
