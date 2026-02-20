# Testing.cmake
# Google Test 测试框架配置

# 防止在 Windows 上覆盖父项目的编译器/链接器设置
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# 添加 Google Test 子目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest ${CMAKE_BINARY_DIR}/googletest)

# Google Test 包含目录
set(GTEST_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googlemock/include
)

# 收集测试源文件
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

# 创建测试可执行文件
add_executable(opengl_tests ${TEST_SOURCES} ${LIB_SOURCES})

# 设置测试包含目录
target_include_directories(opengl_tests PRIVATE
    ${COMMON_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(opengl_tests 
    gtest 
    gtest_main 
    ${PLATFORM_LIBRARIES}
)

# 注册测试到 CTest
include(GoogleTest)
gtest_discover_tests(opengl_tests)
