# OpenGL Demo

一个最小可运行的OpenGL demo项目，使用现代OpenGL 3.3核心配置绘制橙色三角形。包含Python和C++两种实现版本，展示了模块化的项目结构和Shader管理。

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-blue)
![License](https://img.shields.io/badge/license-MIT-blue)

## 特性

- 🎯 **双实现版本**：提供Python和C++两种语言实现
- 🏗️ **模块化架构**：清晰的项目结构，分离头文件、源文件和资源
- 🔧 **CShader类**：封装Shader管理，支持从文件或字符串加载
- 📦 **资源管理**：独立的resources目录管理GLSL着色器、纹理和模型
- 🚀 **跨平台**：支持macOS、Linux和Windows
- 📚 **现代OpenGL**：使用OpenGL 3.3 Core Profile和GLSL 330

## 项目结构

```
opengl_demo/
├── include/              # C++头文件
│   └── shader/
│       └── Shader.h      # CShader类定义
├── src/                  # C++源文件
│   ├── main.cpp          # 主程序入口
│   └── shader/
│       └── Shader.cpp    # CShader类实现
├── resources/            # 资源文件
│   ├── shaders/          # GLSL着色器
│   │   ├── triangle.vs   # 顶点着色器
│   │   └── triangle.fs   # 片段着色器
│   ├── textures/         # 贴图文件（预留）
│   └── models/           # 3D模型（预留）
├── third_party/          # 第三方库（不提交到git）
├── build/               # 构建输出目录
├── opengl_demo.py       # Python版本实现
├── requirements.txt     # Python依赖
├── CMakeLists.txt      # C++构建配置
├── README.md           # 项目说明
└── CLAUDE.md           # 开发者指南
```

## 快速开始

### Python版本

#### 安装依赖

```bash
pip install -r requirements.txt
```

#### 运行

```bash
python3 opengl_demo.py
```

### C++版本

#### 构建项目

```bash
mkdir -p build
cd build
cmake ..
make
```

#### 运行

```bash
./opengl_demo
```

#### 构建选项

**macOS用户**：如果CMake作为应用程序安装，路径通常为：
```
/Applications/CMake.app/Contents/bin/cmake
```

**VS Code用户**：如果遇到"CMake executable error"，创建`.vscode/settings.json`：
```json
{
    "cmake.cmakePath": "/Applications/CMake.app/Contents/bin/cmake"
}
```

## 使用示例

### CShader类使用

```cpp
#include "shader/Shader.h"

// 从文件加载Shader
CShader shader(
    std::string("resources/shaders/triangle.vs"),
    std::string("resources/shaders/triangle.fs")
);

// 使用Shader
shader.use();

// 设置Uniform变量
shader.setBool("useTexture", true);
shader.setFloat("alpha", 0.5f);
shader.setInt("textureUnit", 0);
```

### 自定义Shader

在`resources/shaders/`目录下创建新的shader文件：

**顶点着色器** (`custom.vs`):
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
```

**片段着色器** (`custom.fs`):
```glsl
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0); // 橙色
}
```

然后在代码中加载：
```cpp
CShader shader("resources/shaders/custom.vs", "resources/shaders/custom.fs");
```

## 技术栈

### Python版本
- **PyGame**: 窗口管理和事件处理
- **PyOpenGL**: OpenGL绑定
- **OpenGL 1.2**: 传统固定管线

### C++版本
- **GLFW**: 跨平台窗口和上下文创建
- **GLAD**: OpenGL函数加载器
- **OpenGL 3.3 Core**: 现代可编程管线
- **CMake**: 跨平台构建系统
- **GLSL 330**: 现代着色器语言

## 实现对比

| 特性 | Python版本 | C++版本 |
|------|-----------|---------|
| OpenGL版本 | 1.2 (传统) | 3.3 Core (现代) |
| GLSL版本 | 120 | 330 |
| 窗口管理 | PyGame | GLFW |
| Shader管理 | 内联 | 文件 + CShader类 |
| VAO支持 | 否 | 是 |
| 平台链接 | 自动 | CMake配置 |

## 开发指南

详细开发指南请参考 [CLAUDE.md](CLAUDE.md)，包含：
- 完整的架构说明
- 构建配置细节
- CShader类API文档
- 平台特定配置

## 贡献

欢迎提交Issue和Pull Request！

## 许可证

MIT License

## 作者

kongshan001

## 相关资源

- [Learn OpenGL](https://learnopengl.com/) - 优秀的OpenGL教程
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [OpenGL Wiki](https://www.khronos.org/opengl/wiki)
