cmake_minimum_required(VERSION 3.10)
project(basic_triangle)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)

# Set include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/glm
)

# Collect source files
file(GLOB_RECURSE SOURCES
    "*.cpp"
)

add_executable(basic_triangle ${SOURCES})

target_include_directories(basic_triangle PRIVATE ${OPENGL_INCLUDE_DIR})

# Copy resources to build directory
add_custom_command(TARGET basic_triangle POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../../resources
    ${CMAKE_CURRENT_BINARY_DIR}/resources
)

if(APPLE)
    target_link_libraries(basic_triangle ${OPENGL_LIBRARIES} glfw3 glad "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(UNIX)
    target_link_libraries(basic_triangle ${OPENGL_LIBRARIES} glfw3 glad -ldl -pthread)
elseif(WIN32)
    target_link_libraries(basic_triangle ${OPENGL_LIBRARIES} glfw3 glad)
endif()