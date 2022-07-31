#pragma once
#include <vector>
#include <cassert>
#include <cstdio>
#include <memory>
#include <algorithm>
#include "rendering.h"
#include "stb_image.h"
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
    drawing_params params;
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white) const = 0;
    virtual void draw() const
    {
        params.draw(position.x, position.y);
    };
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
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white) const override;
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
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white) const override;
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
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white) const override;
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
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white) const override;
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
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white) const override;
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
    virtual std::vector<coordinate_t> available_moves(const game_t &game, bool white) const override;
};

struct game_t
{
    game_t()
    {
        for (int i = 1; i <= 8; i++)
            white.emplace_back(new pawn(i, 2, true));
        for (int i = 1; i <= 8; i++)
            black.emplace_back(new pawn(i, 7, false));

        for (int i : {1, 8})
            white.emplace_back(new rook(i, 1, true));
        for (int i : {1, 8})
            black.emplace_back(new rook(i, 8, false));

        for (int i : {2, 7})
            white.emplace_back(new knight(i, 1, true));
        for (int i : {2, 7})
            black.emplace_back(new knight(i, 8, false));

        for (int i : {3, 6})
            white.emplace_back(new bishop(i, 1, true));
        for (int i : {3, 6})
            black.emplace_back(new bishop(i, 8, false));

        white.emplace_back(new queen(4, 1, true));
        black.emplace_back(new queen(4, 8, false));
        white.emplace_back(new king(5, 1, true));
        black.emplace_back(new king(5, 8, false));
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
        for (const auto &piece : white)
        {
            if (piece->position.x == x && piece->position.y == y)
                return piece.get();
        }
        return nullptr;
    }
    piece_t *get_black(uint8_t x, uint8_t y) const
    {
        for (const auto &piece : black)
        {
            if (piece->position.x == x && piece->position.y == y)
                return piece.get();
        }
        return nullptr;
    }

    void draw()
    {
        for (const auto &piece : white)
            piece->draw();
        for (const auto &piece : black)
            piece->draw();
    }
    void update_position(piece_t *piece, uint8_t x, uint8_t y)
    {
        // if there's a piece already there, remove it
        auto same_pos = [x, y](const std::unique_ptr<piece_t> &candidate)
        {
            return coordinate_t{x, y} == candidate->position;
        };
        black.erase(std::remove_if(black.begin(), black.end(), same_pos), black.end());
        white.erase(std::remove_if(white.begin(), white.end(), same_pos), white.end());
        piece->position.x = x;
        piece->position.y = y;
    }
    std::vector<std::unique_ptr<piece_t>> white;
    std::vector<std::unique_ptr<piece_t>> black;
};
