cmake_minimum_required(VERSION 3.0.0)
project(chess VERSION 0.1.0)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL)


function(create_resources dir output)
    # Create empty output file
    file(WRITE ${output} "#include <vector>\n")
    # Collect input files
    file(GLOB bins ${dir}/*.png)
    # Iterate through input files
    foreach(bin ${bins})
        # Get short filename
        string(REGEX MATCH "([^/]+)$" filename ${bin})
        # Replace filename spaces & extension separator for C compatibility
        string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
        # Read hex data from file
        file(READ ${bin} filedata HEX)
        # Convert hex data for C compatibility
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
        # Append data to output file
        file(APPEND ${output} "const std::vector<unsigned char> ${filename} = {${filedata}};\n")
    endforeach()
endfunction()

create_resources(application images.hpp)


file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE DEPENDENCY_FILES ${PROJECT_SOURCE_DIR}/dependencies/*.cpp)
add_executable(chess ${SRC_FILES} ${DEPENDENCY_FILES})
target_include_directories(chess PRIVATE "dependencies" "dependencies/imgui/backends" "dependencies/imgui")

target_link_libraries(chess PRIVATE glfw GLEW::GLEW OpenGL::GL ${CMAKE_DL_LIBS})
