# 架构设计文档

## 概述

opengl_demo 是一个现代 OpenGL 3.3 Core Profile 演示项目，采用模块化设计，职责分离清晰。

## 模块结构

```
opengl_demo/
├── include/              # 头文件
│   ├── core/             # 核心模块
│   │   ├── Camera.h      # 相机控制
│   │   └── Application.h # 应用主类
│   ├── shader/           # 着色器模块
│   │   └── Shader.h      # 着色器管理
│   └── mesh/             # 网格模块
│       ├── Mesh.h        # 网格数据
│       ├── Material.h    # 材质系统
│       ├── Texture.h     # 纹理管理
│       ├── Vertex.h      # 顶点结构
│       ├── ModelLoader.h # 模型加载
│       └── MeshUtils.h   # 几何体工具
├── src/                  # 源文件
│   ├── core/             # 核心模块实现
│   ├── shader/           # 着色器实现
│   ├── mesh/             # 网格实现
│   └── main.cpp          # 入口点
└── resources/            # 资源文件
    ├── shaders/          # GLSL 着色器
    └── models/           # 3D 模型
```

## 核心类职责

### Camera（相机类）

**职责**：管理 3D 相机状态和变换

| 方法 | 功能 |
|------|------|
| `getViewMatrix()` | 获取视图矩阵 |
| `getProjectionMatrix()` | 获取投影矩阵 |
| `processKeyboard()` | 处理键盘移动 |
| `processMouseMovement()` | 处理鼠标旋转 |

**依赖**：glm

### Application（应用类）

**职责**：管理应用程序生命周期

| 方法 | 功能 |
|------|------|
| `initialize()` | 初始化窗口和 OpenGL |
| `run()` | 运行主循环 |
| `close()` | 关闭应用 |
| `render()` | 渲染一帧 |

**依赖**：Camera, Shader, Mesh, Material, GLFW

### CShader（着色器类）

**职责**：编译、链接和管理着色器程序

| 方法 | 功能 |
|------|------|
| `use()` | 激活着色器 |
| `setMat4()` | 设置矩阵 uniform |
| `setVec3()` | 设置向量 uniform |
| `setFloat()` | 设置浮点 uniform |

**特性**：
- 自动编译错误检测（抛出 `ShaderException`）
- Uniform location 缓存

### CMesh（网格类）

**职责**：管理顶点数据和 VAO/VBO

| 方法 | 功能 |
|------|------|
| `draw()` | 渲染网格 |
| `setMaterial()` | 设置材质 |
| `calculateBoundingBox()` | 计算包围盒 |

### CMaterial（材质类）

**职责**：封装渲染属性

| 属性 | 类型 | 说明 |
|------|------|------|
| ambient | vec3 | 环境光颜色 |
| diffuse | vec3 | 漫反射颜色 |
| specular | vec3 | 高光颜色 |
| shininess | float | 光泽度 |

### CModelLoader（模型加载器）

**职责**：从文件加载 3D 模型

| 方法 | 功能 |
|------|------|
| `load()` | 加载模型文件 |
| `isSupported()` | 检查格式支持 |

**支持格式**：OBJ

### CTexture（纹理类）

**职责**：加载和管理 OpenGL 纹理

| 方法 | 功能 |
|------|------|
| `bind()` | 绑定纹理到纹理单元 |
| `setWrapMode()` | 设置包装模式 |
| `setFilterMode()` | 设置过滤模式 |
| `generateMipmaps()` | 生成多级渐远纹理 |

**支持格式**：JPG, PNG, BMP, TGA 等

**示例**：
```cpp
auto texture = std::make_shared<CTexture>(
    "resources/textures/container.png",
    TextureType::Diffuse
);
texture->bind(0);
```

## 数据流

```
main()
  │
  ▼
Application::initialize()
  ├── initWindow()      → GLFW 窗口
  ├── initOpenGL()      → GLAD + OpenGL 状态
  └── initScene()       → Shader + Material + Mesh
        │
        ▼
Application::run()
  ┌─────────────────┐
  │ updateDeltaTime │
  │ processInput    │
  │ render          │
  │   ├─ setGlobalUniforms
  │   └─ renderScene
  │ glfwSwapBuffers │
  └─────────────────┘
```

## 设计原则

1. **RAII**：资源在构造时获取，析构时释放
2. **智能指针**：使用 `shared_ptr` 共享资源
3. **异常安全**：错误通过异常抛出，不静默失败
4. **职责单一**：每个类只负责一件事
5. **依赖注入**：Material 持有 Shader 引用，Mesh 持有 Material 引用

## 扩展点

- **新模型格式**：继承 `IModelLoader`，注册到 `ModelLoaderFactory`
- **新图元类型**：使用 `PrimitiveType` 枚举
- **自定义顶点布局**：使用 `VertexAttributeLayout`
- **纹理系统**：使用 `CTexture` 加载外部纹理，支持多种格式

## 纹理支持

项目支持从外部加载纹理贴图：

```
resources/textures/
├── container.jpg      # 示例纹理
└── container2.png     # 带边框的容器纹理
```

**使用流程**：
1. 加载纹理：`CTexture texture("path/to/texture.png")`
2. 绑定纹理：`texture.bind(0)`
3. 着色器采样：`sampler2D diffuseTexture`
4. 设置 uniform：`shader.setInt("diffuseTexture", 0)`
