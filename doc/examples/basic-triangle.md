# 基础三角形

这是一个最简单的OpenGL Demo示例，展示如何使用CMesh系统创建和渲染一个三角形。

## 代码特点

- 使用`CMesh`类管理顶点数据和OpenGL对象
- 使用内联着色器，避免外部文件依赖
- 展示基础的渲染循环和事件处理
- 包含错误检查和调试输出

## 代码结构

```
basic_triangle/
├── main.cpp
├── CMakeLists.txt
└── README.md
```

## 编译和运行

```bash
mkdir build && cd build
cmake ..
make
./basic_triangle
```

## 效果

显示一个橙色三角形在灰色背景上，ESC键退出。

## 学习要点

1. **CMesh基础使用**：如何创建和管理网格对象
2. **着色器集成**：如何将着色器与网格关联
3. **内存管理**：了解CMesh的RAII机制
4. **错误处理**：如何调试OpenGL错误

## 扩展练习

1. **添加颜色变化**：使用uniform让三角形颜色渐变
2. **添加纹理**：加载纹理并修改片段着色器
3. **多个三角形**：创建多个网格对象并渲染

## 代码示例

### main.cpp核心代码

```cpp
// 简单的三角形示例
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include "mesh/Mesh.h"
#include "shader/Shader.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // 初始化GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Basic Triangle", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // 创建着色器（内联）
    const char* vertexShaderSource = R"GLSL(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";
    
    const char* fragmentShaderSource = R"GLSL(
        #version 330 core
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(1.0, 0.8, 0.2, 1.0);  // 橙色
        }
    )";
    
    CShader shader(vertexShaderSource, fragmentShader);
    
    // 定义三角形顶点（仅位置，无纹理坐标）
    std::vector<Vertex> triangleVertices = {
        Vertex(glm::vec3( 0.0f, 0.5f, 0.0f)),
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f)),
        Vertex(glm::vec3(0.5f, -0.5f, 0.0f))
    };
    
    // 创建网格
    CMesh triangle(triangleVertices);
    
    // 设置简单的顶点布局（仅位置）
    triangle.setVertexLayout(VertexAttributeLayout::PositionOnly());
    
    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 清屏
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 使用着色器
        shader.use();
        
        // 渲染三角形
        triangle.draw();
        
        // 交换缓冲区和事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // 清理资源（CMesh自动清理OpenGL对象）
    glfwTerminate();
    return 0;
}
```

## 关键概念解释

### 1. CMesh类使用
```cpp
// 1. 创建顶点数据
std::vector<Vertex> vertices = {...};

// 2. 创建网格对象
CMesh mesh(vertices);

// 3. 渲染网格
mesh.draw();

// 4. 资源自动清理（析构函数）
```

### 2. Vertex结构
```cpp
Vertex vertex = {
    position: glm::vec3,    // 3D位置
    normal: glm::vec3,      // 法线向量
    texCoords: glm::vec2,    // 纹理坐标
    tangent: glm::vec3,      // 切线向量
    bitangent: glm::vec3     // 副切线向量
};
```

### 3. 顶点属性布局
```cpp
// 仅位置数据
VertexAttributeLayout layout;
layout.addAttribute(VertexAttribute::Position, 3);

// 位置 + 法线
VertexAttributeLayout layout;
layout.addAttribute(VertexAttribute::Position, 3);
layout.addAttribute(VertexAttribute::Normal, 3);

// 完整布局
VertexAttributeLayout layout;
layout.addAttribute(VertexAttribute::Position, 3);
layout.addAttribute(VertexAttribute::Normal, 3);
layout.addAttribute(VertexAttribute::TexCoords, 2);
layout.addAttribute(VertexAttribute::Tangent, 3);
layout.addAttribute(VertexAttribute::Bitangent, 3);
```

### 4. 着色器管理
```cpp
// 从字符串创建（适合简单着色器）
CShader shader(vertexSource, fragmentSource);

// 使用着色器
shader.use();

// 设置uniform变量
shader.setFloat("time", (float)glfwGetTime());
```

## 调试和扩展

### 添加调试输出
```cpp
std::cout << "Created mesh with " << mesh.getVertexCount() << " vertices" << std::endl;
std::cout << "Bounding box: " << bbox.getCenter().x << ", " << bbox.getCenter().y << ", " << bbox.getCenter().z << std::endl;
```

### 添加错误检查
```cpp
// 检查OpenGL错误
GLenum error;
while ((error = glGetError()) != GL_NO_ERROR) {
    std::cout << "OpenGL Error: " << error << std::endl;
}

// CShader类自动处理着色器编译错误
```

### 性能优化
```cpp
// 使用静态顶点数据（适用于静态对象）
static const std::vector<Vertex> triangleVertices = {...};

// 避免在渲染循环中创建对象
CMesh triangle(triangleVertices);  // 构造函数中完成初始化
```

## 下一步

完成基础三角形后，可以继续学习：
1. [纹理贴图](../tutorial/textures.md) - 添加纹理支持
2. [模型加载](../tutorial/model-loading.md) - 加载3D模型文件
3. [着色器进阶](../tutorial/advanced-shaders.md) - 学习更复杂的着色器技术
4. [材质系统](../tutorial/materials-textures.md) - 完整的材质和光照系统