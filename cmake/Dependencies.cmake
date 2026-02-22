# Dependencies.cmake
# 配置项目依赖项

# 使用 GLVND 而非 legacy GL (消除 CMP0072 警告)
set(OpenGL_GL_PREFERENCE GLVND)

# 查找 OpenGL
find_package(OpenGL REQUIRED)

# ============================================================================
# FetchContent - 自动下载依赖
# ============================================================================
include(FetchContent)

# 下载 GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# 下载 GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(glm)

# 下载 Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

# 下载 nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# 设置包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${nlohmann_json_SOURCE_DIR}/include
)

# 公共包含目录
set(COMMON_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/include
    ${OPENGL_INCLUDE_DIR}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${nlohmann_json_SOURCE_DIR}/include
)

# 收集库源文件
file(GLOB_RECURSE LIB_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mesh/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/shader/*.cpp"
)

# 添加 ImGui 源文件
list(APPEND LIB_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# 添加 GLAD 源文件 (OpenGL loader)
list(APPEND LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/third_party/src/glad.c")
