cmake_minimum_required(VERSION 3.0.0)
project(chess VERSION 0.1.0)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/application)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/application)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/application)

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL)


add_executable(chess main.cpp rendering.cpp chess.cpp stb_image.cpp)

target_link_libraries(chess PRIVATE glfw GLEW::GLEW OpenGL::GL)
