# 单元测试指南

本文档介绍如何在 OpenGL Demo 项目中使用 Google Test 框架进行单元测试。

## 目录

1. [概述](#概述)
2. [环境要求](#环境要求)
3. [构建测试](#构建测试)
4. [运行测试](#运行测试)
5. [编写测试](#编写测试)
6. [测试组织](#测试组织)
7. [最佳实践](#最佳实践)

## 概述

项目使用 [Google Test (gtest)](https://github.com/google/googletest) 作为单元测试框架。Google Test 提供：

- 丰富的断言用于各种类型的比较
- 测试夹具（Test Fixtures）用于共享设置/清理
- 参数化测试
- 死亡测试（测试断言/崩溃）
- XML 测试报告输出

## 环境要求

Google Test 已包含在 `third_party/googletest/` 目录中，会随项目自动构建。

无需额外安装。

## 构建测试

### 构建所有内容（包括测试）

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

### 仅构建测试

```bash
cd build
cmake --build . --target opengl_tests
```

## 运行测试

### 运行所有测试

```bash
cd build
./opengl_tests
```

### 运行特定测试套件

```bash
./opengl_tests --gtest_filter=MaterialTest.*
./opengl_tests --gtest_filter=VertexTest.*
```

### 运行特定测试用例

```bash
./opengl_tests --gtest_filter=MaterialTest.DefaultConstructor
```

### 使用 CTest 运行

```bash
cd build
ctest
```

### 输出详细结果

```bash
./opengl_tests --gtest_output=xml:test_results.xml
```

### 列出所有测试

```bash
./opengl_tests --gtest_list_tests
```

## 编写测试

### 基本测试结构

```cpp
#include <gtest/gtest.h>
#include "mesh/YourClass.h"

class YourClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前执行的设置代码
    }
    
    void TearDown() override {
        // 每个测试后执行的清理代码
    }
};

TEST_F(YourClassTest, TestName) {
    // 准备（Arrange）
    YourClass obj;
    
    // 执行（Act）
    auto result = obj.someMethod();
    
    // 断言（Assert）
    EXPECT_EQ(result, expectedValue);
}
```

### 简单测试（无夹具）

```cpp
#include <gtest/gtest.h>

TEST(SimpleTest, BasicAssertion) {
    EXPECT_EQ(1 + 1, 2);
}
```

### 常用断言

```cpp
// 相等性
EXPECT_EQ(actual, expected);    // ==
EXPECT_NE(val1, val2);          // !=
EXPECT_LT(val1, val2);          // <
EXPECT_LE(val1, val2);          // <=
EXPECT_GT(val1, val2);          // >
EXPECT_GE(val1, val2);          // >=

// 布尔值
EXPECT_TRUE(condition);
EXPECT_FALSE(condition);

// 浮点数
EXPECT_FLOAT_EQ(val1, val2);    // 浮点数比较（带容差）
EXPECT_DOUBLE_EQ(val1, val2);   // 双精度比较（带容差）
EXPECT_NEAR(val1, val2, abs_error);  // 在绝对误差范围内

// 字符串
EXPECT_STREQ("hello", str);     // C字符串比较
EXPECT_STRCASEEQ("Hello", str); // 忽略大小写比较

// 指针
EXPECT_EQ(ptr, nullptr);
EXPECT_NE(ptr, nullptr);
```

### 测试夹具

当多个测试需要共享设置时使用夹具：

```cpp
class MeshTest : public ::testing::Test {
protected:
    CMesh* mesh;
    
    void SetUp() override {
        mesh = new CMesh();
    }
    
    void TearDown() override {
        delete mesh;
    }
};

TEST_F(MeshTest, HasCorrectVertexCount) {
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f))
    };
    mesh->setVertices(vertices);
    EXPECT_EQ(mesh->getVertexCount(), 1);
}
```

## 测试组织

### 目录结构

```
opengl_demo/
├── tests/
│   ├── test_vertex.cpp       # Vertex 类测试
│   ├── test_material.cpp     # Material 类测试
│   ├── test_texture.cpp      # Texture 类测试
│   ├── test_mesh.cpp         # Mesh 类测试
│   └── test_model_loader.cpp # ModelLoader 类测试
└── ...
```

### 命名规范

- 测试文件：`test_<类名>.cpp`
- 测试套件：`<类名>Test`
- 测试名称：描述正在测试的内容

示例：
```cpp
// 在 test_material.cpp 中
class MaterialTest : public ::testing::Test { ... };

TEST_F(MaterialTest, DefaultConstructor) { ... }
TEST_F(MaterialTest, SetColors) { ... }
TEST_F(MaterialTest, IsTransparentWhenOpacityBelowOne) { ... }
```

## 最佳实践

### 1. 遵循 AAA 模式

使用 准备-执行-断言 结构：

```cpp
TEST_F(MaterialTest, SetColors) {
    // 准备（Arrange）
    CMaterial material;
    glm::vec3 diffuse(1.0f, 0.5f, 0.2f);
    
    // 执行（Act）
    material.setColors(diffuse, glm::vec3(1.0f), glm::vec3(0.1f));
    
    // 断言（Assert）
    EXPECT_FLOAT_EQ(material.diffuseColor.r, 1.0f);
    EXPECT_FLOAT_EQ(material.diffuseColor.g, 0.5f);
}
```

### 2. 每个测试只验证一个概念

每个测试应该验证一个特定的行为：

```cpp
// 好的做法 - 测试一件事
TEST_F(MaterialTest, IsTransparentWhenOpacityBelowOne) {
    CMaterial material;
    material.setProperties(32.0f, 0.5f, 0.5f);
    EXPECT_TRUE(material.isTransparent());
}

// 避免 - 测试多件事
TEST_F(MaterialTest, AllProperties) {
    CMaterial material;
    // 太多断言...
}
```

### 3. 使用描述性的测试名称

```cpp
// 好的做法
TEST_F(MaterialTest, ReturnsNullptrWhenTextureIndexOutOfBounds) { ... }

// 避免
TEST_F(MaterialTest, TextureTest1) { ... }
```

### 4. 测试边界情况

```cpp
TEST_F(MaterialTest, GetTextureInvalidIndex) {
    CMaterial material;
    EXPECT_EQ(material.getTexture(0), nullptr);     // 空容器
    EXPECT_EQ(material.getTexture(100), nullptr);   // 大索引
}
```

### 5. 测试错误条件

```cpp
TEST_F(ModelLoaderTest, ThrowsOnNonExistentFile) {
    EXPECT_THROW(
        CModelLoader::load("nonexistent.obj"),
        std::exception
    );
}
```

## 持续集成

在 CI 中运行测试：

```bash
#!/bin/bash
set -e

mkdir -p build
cd build
cmake ..
cmake --build . --target opengl_tests
./opengl_tests --gtest_output=xml:test_results.xml
```

## 故障排除

### 链接器错误

如果遇到未定义引用错误，请检查：
1. 源文件是否包含在 CMakeLists.txt 的 `LIB_SOURCES` 中
2. 是否包含了所有必需的头文件

### 测试未运行

检查：
1. 测试文件是否在 `tests/` 目录中
2. 文件扩展名是否为 `.cpp`
3. `TEST()` 或 `TEST_F()` 宏是否正确使用

### 需要 OpenGL 上下文

某些测试可能需要 OpenGL 上下文。对于此类测试：
- 在无头环境中跳过依赖 OpenGL 的测试
- 尽可能使用模拟对象
- 考虑将纯逻辑测试与渲染测试分离

## 添加新测试

1. 在 `tests/` 目录创建新文件 `test_<类名>.cpp`
2. 编写测试代码
3. 重新构建：`cmake --build . --target opengl_tests`
4. 运行测试验证
