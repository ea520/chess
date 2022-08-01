#include <iostream>
#include <cstdlib>
#include <cassert>
#include <array>
#include "rendering.hpp"
#include "chess.hpp"

#include <imgui/imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

constexpr int width = 1000;
constexpr int height = width;

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    game_t *game = (game_t *)glfwGetWindowUserPointer(window);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !game->promote)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        uint8_t x = uint8_t(xpos / width * 8.) + 1;
        uint8_t y = uint8_t(9 - (ypos / height * 8.));
        // printf("%c%d\n", 'A' + char(x - 1), 8 - int(y));
        // fflush(stdout);
        if (game->current_piece && std::find(game->moves.begin(), game->moves.end(), coordinate_t{x, y}) != game->moves.end())
        {
            if (game->current_piece->ispawn() && game->white_turn && y == 8)
            {
                game->update_position(game->current_piece, x, y);
                game->promote = true;
                return;
            }
            else if (game->current_piece->ispawn() && !game->white_turn && y == 1)
            {
                game->update_position(game->current_piece, x, y);
                game->promote = true;
                return;
            }
            else
                game->promote = false;
            game->update_position(game->current_piece, x, y);
            game->moves = {};
            game->white_turn = !game->white_turn;
            return;
        }
        if (game->white_turn)
            game->current_piece = game->get_white(x, y);
        else
            game->current_piece = game->get_black(x, y);
        if (game->current_piece)
        {
            game->moves = game->current_piece->available_moves(*game, game->white_turn, game->enpassant);
            return;
        }
        game->moves = {};
    }
}

static void
error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam);
int main()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(width, height, "Chess", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    constexpr int board_width = 8, board_height = 8;

    std::array<uint8_t, 4> brown{0xd2, 0x69, 0x1e, 0xff};
    std::array<uint8_t, 4> white{0xff, 0xff, 0xff, 0xff};
    std::array<uint8_t, 4> blue_transparent{0x05, 0x10, 0xff, 0x88};
    std::array<std::array<std::array<uint8_t, 4>, board_width>, board_height> pixels;
    for (unsigned j = 0; j < board_height; j++)
        for (unsigned i = 0; i < board_width; i++)
        {
            pixels[j][i] = (i + j) % 2 ? brown : white;
        }

    drawing_params board = setup_square(pixels.data(), board_width, board_height, 2.f, GL_NEAREST);
    drawing_params square = setup_square(blue_transparent.data(), 1, 1, 0.25f, GL_NEAREST);
    game_t game;
    glfwSetWindowUserPointer(window, &game);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    while (!glfwWindowShouldClose(window))
    {
        // glfwPollEvents();
        glfwWaitEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (game.promote)
        {
            ImGui::Begin(game.white_turn ? "White pawn promotion" : "Black pawn promotion");
            int e = -1;
            ImGui::RadioButton("Queen", &e, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Rook", &e, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Bishop", &e, 2);
            ImGui::SameLine();
            ImGui::RadioButton("Knight", &e, 3);
            if (e != -1)
            {
                auto &array = game.white_turn ? game.white_pieces : game.black_pieces;
                switch (e)
                {
                case 0:
                    array.emplace_back(new queen(game.current_piece->position.x, game.current_piece->position.y, game.white_turn));
                    break;
                case 1:
                    array.emplace_back(new rook(game.current_piece->position.x, game.current_piece->position.y, game.white_turn));
                    break;
                case 2:
                    array.emplace_back(new bishop(game.current_piece->position.x, game.current_piece->position.y, game.white_turn));
                    break;
                case 3:
                    array.emplace_back(new knight(game.current_piece->position.x, game.current_piece->position.y, game.white_turn));
                    break;
                default:
                    assert(false);
                    break;
                }
                game.delete_current_piece();
                game.promote = false;
                game.white_turn = !game.white_turn;
            }

            ImGui::End();
        }

        ImGui::Render();
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        board.draw();
        game.draw();
        for (coordinate_t position : game.moves)
        {
            square.draw(position.x, position.y);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        std::cout << "Source: API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cout << "Source: Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cout << "Source: Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cout << "Source: Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cout << "Source: Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cout << "Source: Other";
        break;
    }
    std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "Type: Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "Type: Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "Type: Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "Type: Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "Type: Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cout << "Type: Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cout << "Type: Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cout << "Type: Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "Type: Other";
        break;
    }
    std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "Severity: high";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "Severity: medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "Severity: low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cout << "Severity: notification";
        break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}
