# CMakeLists.txt
# OpenGL Demo 项目主构建配置

cmake_minimum_required(VERSION 3.10)
project(opengl_demo VERSION 1.0.0 LANGUAGES CXX C)

# ============================================================================
# 全局设置
# ============================================================================

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 启用测试
enable_testing()

# 设置 cmake 模块路径
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ============================================================================
# 依赖项
# ============================================================================

include(Dependencies)

# ============================================================================
# 平台配置
# ============================================================================

include(Platform)

# ============================================================================
# 主可执行文件
# ============================================================================

# 收集主程序源文件
file(GLOB MAIN_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# 创建可执行文件
add_executable(opengl_demo ${MAIN_SOURCE} ${LIB_SOURCES})

# 设置包含目录
target_include_directories(opengl_demo PRIVATE ${COMMON_INCLUDE_DIRS})

# 链接库（FetchContent 目标）
target_link_libraries(opengl_demo
    ${OPENGL_LIBRARIES}
    glfw
    glm::glm
)

# 平台特定链接
if(APPLE)
    target_link_libraries(opengl_demo
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(opengl_demo
        -ldl
        -lpthread
    )
elseif(WIN32)
    target_link_libraries(opengl_demo opengl32)
endif()

# 复制资源到构建目录
add_custom_command(TARGET opengl_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_BINARY_DIR}/resources
    COMMENT "Copying resources to build directory"
)

# ============================================================================
# 测试配置
# ============================================================================

include(Testing)
