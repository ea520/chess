#include <GL/glew.h>
#include "rendering.hpp"

// Shader sources
const GLchar *vertexSource = R"glsl(
    #version 330 core
    in vec2 position;
    in vec2 texcoord;
    out vec2 Texcoord;
    uniform vec2 offset;
    void main()
    {
        Texcoord = texcoord;
        gl_Position = vec4(position + offset, 0.0, 1.0);
    }
)glsl";

const GLchar *fragmentSource = R"glsl(
    #version 330 core
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D board;
    void main()
    {
        outColor = texture(board, Texcoord);
    }
)glsl";

shader::shader(const char *vertex_src, const char *fragment_src)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_src, nullptr);
    glCompileShader(vertexShader);

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_src, nullptr);
    glCompileShader(fragmentShader);

    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    use();
}
void shader::use() const
{
    glUseProgram(shaderProgram);
}
GLuint shader::get_id() const { return shaderProgram; }

vao::vao()
{
    // Create Vertex Array Object
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);
    bind();
}
void vao::bind() const
{
    glBindVertexArray(id);
}
const GLuint square_ebo::elements[6] = {
    0, 1, 2,
    2, 3, 0};

square_ebo::square_ebo()
{
    // Create an element array
    glGenBuffers(1, &ebo);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}
void square_ebo::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

square_vbo::square_vbo(float size)
{
    struct vertex
    {
        std::array<float, 2> pos, tex;
    };

    std::array<vertex, 4> square;
    square[0].tex = {0.f, 1.f};
    square[1].tex = {1.f, 1.f};
    square[2].tex = {1.f, 0.f};
    square[3].tex = {0.f, 0.f};

    square[0].pos = {0.0f, 0.0f}; // bottom left
    square[1].pos = {size, 0.0f}; // bottom right
    square[2].pos = {size, size}; // top right
    square[3].pos = {0.0f, size}; // top left

    glGenBuffers(1, &id);
    bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square.data(), GL_STATIC_DRAW);
}
void square_vbo::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

texture::texture(GLuint shaderProgram, const char *texture_name, const void *pixels, int width, int height, GLenum interpolation)
{
    // Load textures
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // glUniform1i(glGetUniformLocation(shaderProgram, texture_name), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
}

void texture::bind() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
}

void drawing_params::draw(uint8_t x, uint8_t y) const
{
    VAO.bind();
    vbo.bind();
    Shader.use();
    glUniform2f(glGetUniformLocation(Shader.get_id(), "offset"), 0.25f * (x - 5), 0.25f * (y - 5));
    tex.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void set_layout(GLuint shaderProgram)
{
    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
}

drawing_params setup_square(const void *image, int width, int height, float size, GLenum interpolation)
{
    vao VAO;
    square_vbo vbo(size);

    static square_ebo ebo; // same for every square
    ebo.bind();

    static shader square_texture_shader(vertexSource, fragmentSource); // same shader for every square

    static GLuint shaderProgram = square_texture_shader.get_id();

    set_layout(shaderProgram);
    texture tex(shaderProgram, "tex", image, width, height, interpolation);
    return drawing_params{VAO, vbo, ebo, square_texture_shader, tex};
};