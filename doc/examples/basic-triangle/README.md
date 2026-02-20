# 基础三角形

这是最简单的OpenGL示例，展示如何使用CMesh系统创建和渲染一个三角形。

## 🎯 程序特点

- 使用`CMesh`类管理几何数据
- 内联着色器，无需外部文件
- 基础的错误处理
- 展示了标准的OpenGL渲染循环

## 📝 代码结构

```
basic_triangle/
├── CMakeLists.txt    # CMake配置
├── main.cpp          # 主程序
└── README.md        # 项目说明
└── resources/       # 资源文件（复制自主目录）
```

## 🚀 编译运行

```bash
cd doc/examples/basic-triangle
mkdir build && cd build
cmake ..
make
./basic_triangle
```

## 📖 使用说明

### 控制
- **ESC**: 退出程序

### 示例输出
- 窗口标题: "Basic Triangle"
- 背景色: 灰色背景
- 左侧: 橙色三角形
- 按ESC键退出程序

## 🎯 核心概念

### 1. CMesh类
```cpp
// 1. 创建顶点数据
std::vector<Vertex> vertices = {
    Vertex(glm::vec3(-0.5f, -0.5f, 0.0f)),
    Vertex(glm::vec3(0.5f, -0.5f, 0.0f)),
    Vertex(glm::vec3(0.5f, -0.5f, 0.0f))
};

// 2. 创建网格对象
CMesh triangle(vertices);

// 3. 渲染网格
triangle.draw();
```

### 2. Vertex结构
```cpp
struct Vertex {
    glm::vec3 position;    // 3D位置坐标
    glm::vec3 normal;      // 法线向量（本示例中未使用）
    glm::vec2 texCoords;  // 2D纹理坐标（本示例中未使用）
    glm::vec3 tangent;    // 切线向量（本示例中未使用）
    glm::vec3 bitangent;   // 副切线向量（本示例中未使用）
    
    // 便利构造函数
    Vertex(const glm::vec3& pos) : position(pos), normal(0.0f), texCoords(0.0f), tangent(0.0f), bitangent(0.0f) {}
    Vertex() : position(0.0f), normal(0.0f), texCoords(0.0f), tangent(0.0f), bitangent(0.0f) {}
};
```

### 3. 内联着色器
顶点着色器（仅包含位置数据）：
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
```

片段着色器（输出固定橙色）：
```glsl
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.8, 0.2, 1.0); // 橙色
}
```

## 🔧 扩展练习

### 1. 添加颜色变化
修改片段着色器，让三角形颜色随时间变化：
```glsl
#version 330 core
out vec4 FragColor;
uniform float time;

void main() {
    FragColor = vec4(
        (sin(time) * 0.5f + 0.5f),
        (sin(time * 0.5f + 1.0f) * 0.5f + 0.3f),
        0.8, // 固定绿色
        1.0
    );
}
```

### 2. 添加纹理支持
```cpp
// 1. 添加纹理坐标到顶点数据
std::vector<Vertex> vertices = {
    Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.5f), glm::vec2(0.0f, 1.0f)),
    Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.5f), glm::vec2(1.0f, 1.0f)),
    Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
    Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 1.0f), glm::vec2(0.0f, 1.0f))
};

// 2. 设置纹理布局
triangle.setVertexLayout(VertexAttributeLayout::PositionNormalTex());

// 3. 加载纹理
auto texture = std::make_shared<CTexture>("resources/textures/uv.jpg", TextureType::Diffuse);
auto material = std::make_shared<CMaterial>("TriangleMaterial");
material->addTexture(texture);
triangle.setMaterial(material);

// 4. 修改着色器使用纹理
```

### 3. 添加多个三角形
```cpp
// 定义多个三角形的顶点
std::vector<Vertex> vertices = {
    // 第一个三角形
    Vertex(glm::vec3(-0.5f, 0.0f, 0.0f)), glm::vec3(-1.0f, -0.5f, 0.0f)), glm::vec2(0.0f, 1.0f)),
    Vertex(glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(1.0f, -0.5f, 0.0f)), glm::vec2(1.0f, 1.0f)),
    
    // 第二个三角形
    Vertex(glm::vec3(-0.5f, 0.5f, 0.0f)), glm::vec3(-0.5f, 0.5f, 0.0f)), glm::vec2(0.0f, 0.0f)),
    Vertex(glm::vec3(0.5f, 0.5f, 0.0f)), glm::vec3(0.5f, 0.5f, 0.0f)), glm::vec2(1.0f, 0.0f)),
    
    // 第三个三角形
    Vertex(glm::vec3(0.5f, -0.5f, 0.0f)), glm::vec3(0.5f, 0.5f, 0.0f)), glm::vec2(1.0f, 0.0f)),
    Vertex(glm::vec3(1.0f, 0.5f, 0.0f)), glm::vec3(1.0f, 0.5f, 0.0f)), glm::vec2(0.0f, 0.0f))
};

// 使用不同的颜色
std::vector<CMaterial> materials = {
    createMaterial("RedMaterial", glm::vec3(1.0f, 0.0f, 0.0f)),
    createMaterial("GreenMaterial", glm::vec3(0.0f, 1.0f, 0.0f)),
    createMaterial("BlueMaterial", glm::vec3(0.0f, 0.0f, 1.0f))
};

std::vector<std::shared_ptr<CMesh>> triangles = {
    std::make_shared<CMesh>(vertices[0], 3),  // 第一个三角形
    std::make_shared<CMesh>(vertices[3], 3),  // 第二个三角形
    std::make_shared<CMesh>(vertices[6], 3),  // 第三个三角形
};

// 设置不同材质
for (int i = 0; i < triangles.size(); ++i) {
    triangles[i]->setMaterial(materials[i]);
}

// 渲染所有三角形
for (auto& triangle : triangles) {
    triangle.draw();
}
```

## 🎯 学习路径

完成基础三角形后，建议按以下顺序学习：

1. **[材质和纹理](../tutorial/materials-textures.md)** - 学习纹理映射和材质系统
2. **[模型加载](../tutorial/model-loading.md)** - 学习从文件加载3D模型
3. [着色器进阶](../tutorial/advanced-shaders.md)** - 学习高级着色器技术
4. [几何体生成](../tutorial/geometry-generation.md)** - 学习程序化几何体生成

## 🐛 常见问题

### Q: 三角形没有显示
**A:** 可能的原因：
- 着色器编译错误
- 顶点数据顺序错误
- OpenGL上下文未正确初始化
- 模型矩阵未设置

**解决方法：**
1. 检查控制台错误输出
2. 使用调试输出验证顶点数据
3. 确认VAO正确绑定

### Q: 程序崩溃
**A:** 可能的原因：
- 空指针错误
- 内存访问违规
- OpenGL状态机配置错误

**解决方法：**
1. 使用调试器设置断点
2. 检查对象生命周期
3. 使用智能指针管理内存

### Q: 如何添加键盘控制？
```cpp
// 在渲染循环中添加
processInput(window);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W) {
        camera.processKeyboard(GLFW_PRESS);
    }
    // 处理其他按键...
}

// 注册回调
glfwSetKeyCallback(window, key_callback);
```

现在开始你的OpenGL学习之旅吧！