#pragma once
#include <unordered_set>
#include <cassert>
#include <cstdio>
#include <memory>
#include <algorithm>
#include "rendering.hpp"
#include <stb_image.hpp>
#include <iostream>
#include <array>
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
    bool operator!=(coordinate_t right) const
    {
        return x != right.x || y != right.y;
    }
};
struct game_t;

enum class piece_type
{
    invalid,
    pawn,
    rook,
    king,
    bishop,
    queen,
    knight,
};
struct piece_t
{
    piece_t() : position({0, 0}), type(piece_type::invalid)
    {
    }
    piece_t(uint8_t x, uint8_t y, bool white, piece_type t, bool *check = nullptr) : position({x, y}), type(t), white(white)
    {
        std::string filename;
        stbi_set_flip_vertically_on_load(!white);
        filename = white ? "white-" : "black-";
        switch (t)
        {
        case piece_type::pawn:
            filename += "pawn";
            break;
        case piece_type::rook:
            filename += "rook";
            break;
        case piece_type::king:
            filename += "king";
            break;
        case piece_type::queen:
            filename += "queen";
            break;
        case piece_type::bishop:
            filename += "bishop";
            break;
        case piece_type::knight:
            filename += "knight";
            break;
        default:
            break;
        }
        filename += ".png";
        int w, h;

        auto data = stbi_load(filename.data(), &w, &h, nullptr, 4);
        params = setup_square(data, w, h, 0.25f);
        stbi_image_free(data);
        position = {x, y};
    }
    std::vector<coordinate_t> available_moves(const game_t &game, bool white, coordinate_t enpassant, bool *check = nullptr) const;

    void draw() const
    {
        if (!isinvalid())
            params.draw(position.x, position.y);
    };
    inline bool ispawn() const { return type == piece_type::pawn; }
    inline bool isrook() const { return type == piece_type::rook; }
    inline bool isking() const { return type == piece_type::king; }
    inline bool isqueen() const { return type == piece_type::queen; }
    inline bool isbishop() const { return type == piece_type::bishop; }
    inline bool isknight() const { return type == piece_type::knight; }
    inline bool isinvalid() const { return type == piece_type::invalid; }
    inline bool iswhite() const { return white; }
    operator bool() { return !isinvalid(); }
    piece_type get_type() const { return type; }
    coordinate_t get_position() const { return position; }
    // sets has_moved to true
    void set_position(game_t &g, uint8_t x, uint8_t y);

    std::vector<coordinate_t> pawn_available_moves(const game_t &game, bool white, coordinate_t enpassant) const;
    std::vector<coordinate_t> rook_available_moves(const game_t &game, bool white, coordinate_t enpassant) const;
    std::vector<coordinate_t> king_available_moves(const game_t &game, bool white, coordinate_t enpassant) const;
    std::vector<coordinate_t> queen_available_moves(const game_t &game, bool white, coordinate_t enpassant) const;
    std::vector<coordinate_t> bishop_available_moves(const game_t &game, bool white, coordinate_t enpassant) const;
    std::vector<coordinate_t> knight_available_moves(const game_t &game, bool white, coordinate_t enpassant) const;

private:
    coordinate_t position;
    bool has_moved = false;
    drawing_params params;
    piece_type type;
    bool white;
};

struct game_t
{
    game_t();
    piece_t get(uint8_t x, uint8_t y) const;
    piece_t &get(uint8_t x, uint8_t y);
    piece_t get(coordinate_t p) const { return get(p.x, p.y); }
    piece_t &get(coordinate_t p) { return get(p.x, p.y); }

    piece_t get_black(uint8_t x, uint8_t y) const
    {
        const piece_t &p = get(x, y);
        return !p.iswhite() ? p : piece_t();
    };
    piece_t get_white(uint8_t x, uint8_t y) const
    {
        const piece_t &p = get(x, y);
        return p.iswhite() ? p : piece_t();
    }
    inline piece_t get_current_piece() const { return current_piece; }
    void set_current_piece(piece_t p)
    {
        current_piece = p;
        if (p.isking())
            if (p.iswhite())
                white_king = p.get_position();
            else
                black_king = p.get_position();
    }

    void draw();
    void move(uint8_t x, uint8_t y);

    bool in_check(bool white) const;
    bool in_check_mate(bool white) const;

    std::array<std::array<piece_t, 8>, 8> board;
    coordinate_t enpassant{0, 0};
    std::vector<coordinate_t> moves;
    bool white_turn = true;
    bool promote = false;
    bool white_check = false;
    bool black_check = false;

    coordinate_t white_king;
    coordinate_t black_king;

private:
    piece_t current_piece;
};
