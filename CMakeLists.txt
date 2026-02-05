cmake_minimum_required(VERSION 3.10)
project(opengl_demo)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)

# Set include directories before creating executable
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/include
)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib)

# Collect all source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*/*.cpp"
)

add_executable(opengl_demo ${SOURCES})

target_include_directories(opengl_demo PRIVATE ${OPENGL_INCLUDE_DIR})

# Copy resources to build directory
add_custom_command(TARGET opengl_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_BINARY_DIR}/resources
)

if(APPLE)
    target_link_libraries(opengl_demo ${OPENGL_LIBRARIES} glfw3 glad "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(UNIX)
    target_link_libraries(opengl_demo ${OPENGL_LIBRARIES} glfw3 glad -ldl -pthread)
elseif(WIN32)
    target_link_libraries(opengl_demo ${OPENGL_LIBRARIES} glfw3 glad)
endif()
