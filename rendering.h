#pragma once
#include <GL/glew.h>
#include <array>
class shader
{
public:
    shader(const char *vertex_src, const char *fragment_src);
    shader() : shaderProgram(-1){};
    void use() const;
    GLuint get_id() const;

private:
    GLuint shaderProgram;
};

class vao
{
public:
    vao();
    void bind() const;

private:
    GLuint id;
};

class square_ebo
{
public:
    square_ebo();
    void bind() const;

private:
    GLuint ebo;
    constexpr static GLuint elements[] = {
        0, 1, 2,
        2, 3, 0};
};

class square_vbo
{
public:
    square_vbo(float size = 0);
    void bind() const;

private:
    GLuint id;
};
class texture
{
public:
    texture(GLuint shaderProgram, const char *texture_name, void *pixels, int width, int height, GLenum interpolation = GL_NEAREST);
    texture() : id(-1){};
    void bind() const;

private:
    GLuint id;
};

struct drawing_params
{
    vao VAO;
    square_vbo vbo;
    square_ebo ebo;
    shader Shader;
    texture tex;
    void draw(uint8_t x = 1, uint8_t y = 1) const;
};

drawing_params setup_square(void *image, int width, int height, float size);