cmake_minimum_required(VERSION 3.0.0)
project(chess VERSION 0.1.0)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/application)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/application)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/application)

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE DEPENDENCY_FILES ${PROJECT_SOURCE_DIR}/dependencies/*.cpp)
add_executable(chess ${SRC_FILES} ${DEPENDENCY_FILES})
target_include_directories(chess PRIVATE "dependencies" "dependencies/imgui/backends" "dependencies/imgui")

target_link_libraries(chess PRIVATE glfw GLEW::GLEW OpenGL::GL ${CMAKE_DL_LIBS})
