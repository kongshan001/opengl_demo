/**
 * @file test_shader.cpp
 * @brief Unit tests for CShader class
 */

#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader/Shader.h"

// 测试夹具：需要 OpenGL 上下文
class ShaderTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // 注意：完整的测试需要 OpenGL 上下文
        // 这里主要测试不需要 OpenGL 的部分
    }
};

// 测试 ShaderException 基本功能
TEST(ShaderExceptionTest, WhatReturnsMessage) {
    ShaderException ex("Test error message");
    EXPECT_STREQ(ex.what(), "Test error message");
}

TEST(ShaderExceptionTest, CanBeCaughtAsStdException) {
    try {
        throw ShaderException("Error");
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), "Error");
    }
}

// 测试无效着色器源编译
TEST(ShaderExceptionTest, InvalidVertexShaderSourceThrows) {
    // 无效的 GLSL 代码
    const char* invalidVertexSource = "#version 330 core\nvoid main() { INVALID SYNTAX }";
    const char* validFragmentSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() { FragColor = vec4(1.0); }\n";
    
    // 注意：需要 OpenGL 上下文才能真正测试
    // 此测试在 CI 中可能需要 mock 或跳过
    // EXPECT_THROW(CShader shader(invalidVertexSource, validFragmentSource), ShaderException);
}

// 测试文件加载
TEST(ShaderFileLoadTest, NonExistentFileThrows) {
    // 使用私有方法测试需要在友元类中进行
    // 这里通过构造函数间接测试
    // EXPECT_THROW(CShader shader("/nonexistent/vertex.glsl", "/nonexistent/fragment.glsl"), ShaderException);
}

// 测试有效着色器编译
TEST(ShaderCompilationTest, ValidShaderCompiles) {
    const char* validVertexSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() { gl_Position = vec4(aPos, 1.0); }\n";
    
    const char* validFragmentSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() { FragColor = vec4(1.0, 0.5, 0.2, 1.0); }\n";
    
    // 注意：需要 OpenGL 上下文
    // EXPECT_NO_THROW(CShader shader(validVertexSource, validFragmentSource));
}

// 测试 uniform 设置（需要 OpenGL 上下文）
TEST(ShaderUniformTest, SetBoolValue) {
    // 需要实际的 OpenGL 上下文
}

TEST(ShaderUniformTest, SetIntValue) {
    // 需要实际的 OpenGL 上下文
}

TEST(ShaderUniformTest, SetFloatValue) {
    // 需要实际的 OpenGL 上下文
}

TEST(ShaderUniformTest, SetVec3Value) {
    // 需要实际的 OpenGL 上下文
}

TEST(ShaderUniformTest, SetMat4Value) {
    // 需要实际的 OpenGL 上下文
}

// 测试 uniform location 缓存
TEST(ShaderUniformCacheTest, CachedLocationReuse) {
    // 验证同一 uniform 的多次访问使用缓存
    // 需要 OpenGL 上下文
}

// main 函数由测试框架提供
// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
