# Dependencies.cmake
# 配置项目依赖项

# 查找 OpenGL
find_package(OpenGL REQUIRED)

# 设置包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm
)

# 设置库目录
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib)

# 公共包含目录
set(COMMON_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm
    ${OPENGL_INCLUDE_DIR}
)

# 收集库源文件
file(GLOB_RECURSE LIB_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mesh/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/shader/*.cpp"
)
