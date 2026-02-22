# OpenGL Demo

一个现代 OpenGL 3.3 Core Profile 演示项目，展示了模块化架构、RAII 资源管理和生产级代码质量。

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-blue)
![C++](https://img.shields.io/badge/C++-11-blue)
![License](https://img.shields.io/badge/license-MIT-blue)

## 特性

### 渲染系统
- **模块化架构**：清晰的项目结构，分离头文件、源文件和资源
- **完整 Phong 光照**：环境光、漫反射、镜面反射
- **多光源系统**：支持 4 个点光源，距离衰减
- **动态光源**：光源轨道旋转动画
- **多种几何体**：立方体、球体、圆柱体、圆锥体、圆环、胶囊体
- **多纹理支持**：漫反射、镜面反射、法线贴图
- **线框模式**：可切换线框/填充渲染

### 交互和调试
- **ImGui 调试界面**：性能统计、光源控制、纹理控制
- **键盘控制**：WASD+QE 移动，鼠标视角，滚轮缩放
- **显示模式**：可选择性显示不同几何体
- **场景序列化**：JSON 格式保存/加载场景

### 性能优化
- **RAII 资源管理**：使用智能指针和异常安全保证
- **视锥体剔除**：剔除不在视野内的物体
- **Uniform location 缓存**：避免重复 OpenGL 查询
- **索引化顶点**：消除重复（OBJ 加载器）
- **包围盒预计算**：加速碰撞检测

### 跨平台和测试
- **跨平台**：支持 macOS、Linux 和 Windows
- **异常处理**：完善的 `ShaderException` 和 `ModelLoadException` 错误处理
- **单元测试**：Google Test 测试框架集成（103+ 测试）
- **自动化构建**：使用 CMake 和 FetchContent 自动下载依赖

## 项目结构

```
opengl_demo/
├── include/              # C++ 头文件
│   ├── core/
│   │   ├── Application.h      # 主应用类
│   │   ├── Camera.h          # 摄像机控制
│   │   ├── Frustum.h         # 视锥体剔除
│   │   └── SceneSerializer.h # 场景序列化
│   ├── shader/
│   │   └── Shader.h          # CShader 类 + ShaderException
│   └── mesh/
│       ├── Mesh.h            # CMesh 类（顶点、索引、包围盒）
│       ├── Material.h        # CMaterial 材质系统
│       ├── Texture.h         # CTexture 纹理加载
│       ├── Vertex.h          # CVertex 顶点结构
│       ├── MeshUtils.h       # 几何体生成工具
│       └── ModelLoader.h     # CModelLoader + ModelLoadException
├── src/                  # C++ 源文件
│   ├── main.cpp           # 主程序入口
│   ├── core/
│   │   ├── Application.cpp
│   │   ├── Camera.cpp
│   │   ├── Frustum.cpp
│   │   └── SceneSerializer.cpp
│   ├── shader/
│   │   └── Shader.cpp
│   └── mesh/
│       ├── Mesh.cpp
│       ├── Material.cpp
│       ├── Texture.cpp
│       ├── MeshUtils.cpp
│       └── ModelLoader.cpp
├── resources/            # 资源文件
│   ├── shaders/          # GLSL 着色器
│   │   ├── mesh.vs
│   │   ├── phong.fs
│   │   ├── multi_light.fs
│   │   └── light_source.fs
│   ├── textures/         # 纹理文件
│   └── models/           # 3D 模型（OBJ 格式）
├── tests/                # 单元测试
│   ├── test_shader.cpp
│   ├── test_material.cpp
│   └── test_vertex.cpp
├── third_party/          # 第三方库
│   └── src/
│       └── glad.c        # OpenGL 加载器
├── doc/                  # 文档
│   ├── api/              # API 参考
│   ├── examples/         # 示例代码
│   ├── changelog.md      # 变更日志
│   ├── progress.md       # 开发进度
│   └── todo.md          # TODO 列表
├── cmake/                # CMake 模块
├── CMakeLists.txt        # 构建配置
└── README.md
```

## 快速开始

### 依赖项

- CMake 3.10+
- C++11 编译器
- OpenGL 3.3+
- GLFW（系统安装）

### 构建项目

```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### 运行

```bash
./opengl_demo
```

## 控制说明

### 键盘控制

| 按键 | 功能 |
|------|------|
| **W/A/S/D** | 前后左右移动 |
| **Q/E** | 上下移动 |
| **空格** | 暂停/继续动画 |
| **R** | 重置摄像机位置 |
| **L** | 切换光源动画 |
| **F1** | 显示控制帮助 |
| **ESC** | 退出程序 |

### 显示模式

| 按键 | 功能 |
|------|------|
| **0** | 显示全部几何体 |
| **1** | 只显示立方体 |
| **2** | 只显示球体 |
| **3** | 只显示圆柱体 |
| **4** | 只显示圆锥体 |
| **5** | 切换线框/填充模式 |

### 鼠标控制

| 操作 | 功能 |
|------|------|
| **移动鼠标** | 旋转视角 |
| **滚轮** | 缩放（调整 FOV） |

### UI 界面

| 按键 | 功能 |
|------|------|
| **Tab** | 切换 ImGui 界面显示 |

**菜单栏功能**：
- **窗口**：打开/关闭性能统计、光源控制窗口
- **显示**：切换显示模式（全部/立方体/球体/圆柱体/圆锥体）、线框模式

**光源控制**：
- 实时调整光源颜色、位置
- 控制光源轨道参数（半径、高度、速度）
- 开关光源动画

### 场景布局

```
    [圆锥体]
       (4)
       
[立方体]  [球体]  [圆柱体]
  (1)      (2)      (3)
       
      [地面]
```

### 光源

场景包含 3 个动态点光源：
- **白光** - 主光源，暖白色
- **蓝光** - 冷色调补光
- **橙光** - 暖色调补光

光源会自动围绕场景旋转，按 **L** 键可切换动画。

### 运行测试

```bash
./opengl_tests
```

## 核心类

### CShader - 着色器管理

```cpp
#include "shader/Shader.h"

// 从文件加载（失败时抛出 ShaderException）
CShader shader(
    "resources/shaders/triangle.vs",
    "resources/shaders/triangle.fs"
);

shader.use();
shader.setMat4("model", modelMatrix);
shader.setVec3("lightColor", glm::vec3(1.0f));

// Uniform location 自动缓存，提升性能
```

### CMesh - 网格数据

```cpp
#include "mesh/Mesh.h"

// 创建三角形网格
std::vector<Vertex> vertices = {...};
std::vector<unsigned int> indices = {...};

CMesh mesh(vertices, indices);
mesh.calculateBoundingBox();
mesh.calculateNormals();

// 渲染
mesh.bind();
mesh.draw();
```

### CModelLoader - 模型加载

```cpp
#include "mesh/ModelLoader.h"

try {
    auto meshes = CModelLoader::load("resources/models/cube.obj");
    for (const auto& mesh : meshes) {
        mesh->draw(shader);
    }
} catch (const ModelLoadException& e) {
    std::cerr << "Model load error: " << e.what() << std::endl;
}
```

## 架构设计

### 异常处理

```
std::exception
    └── ShaderException    // 着色器编译/链接/加载错误
    └── ModelLoadException // 模型加载错误
```

所有异常都继承自 `std::exception`，可通过 `what()` 获取错误信息。

### RAII 资源管理

- **CShader**: 析构时自动调用 `glDeleteProgram()`
- **CMesh**: 析构时自动删除 VAO/VBO/EBO
- **ModelLoaderFactory**: 返回 `std::unique_ptr`，无内存泄漏

### 性能优化

- Uniform location 缓存（`std::unordered_map`）
- 索引化顶点消除重复（OBJ 加载器）
- 包围盒预计算

## 技术栈

| 组件 | 技术 |
|------|------|
| 窗口管理 | GLFW 3.3.8 |
| OpenGL 加载 | GLAD |
| 数学库 | GLM 0.9.9.8 |
| JSON 序列化 | nlohmann/json 3.11.3 |
| 调试界面 | Dear ImGui (docking) |
| 构建系统 | CMake 3.10+ |
| 测试框架 | Google Test |
| 图像加载 | stb_image |

## 版本信息

- **OpenGL**: 3.3 Core Profile
- **GLSL**: 330
- **C++**: 11/14
- **CMake**: 3.10+
- **当前版本**: v0.3.0-dev

## 文档

### 快速导航

- 📄 [变更日志](doc/changelog.md) - 项目的所有重要变更
- 📊 [开发进度](doc/progress.md) - 已完成需求和功能统计
- ✅ [TODO 列表](doc/todo.md) - 未来计划和待完成任务

### 详细文档

- [API 参考](doc/api/)
  - [CShader](doc/api/CShader.md)
  - [CMesh](doc/api/CMesh.md)
- [测试指南](doc/testing.md)
- [开发指南](CLAUDE.md)

## 开发指南

详细开发指南请参考 [CLAUDE.md](CLAUDE.md)。

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

MIT License

## 相关资源

- [Learn OpenGL](https://learnopengl.com/)
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [OpenGL Wiki](https://www.khronos.org/opengl/wiki)
