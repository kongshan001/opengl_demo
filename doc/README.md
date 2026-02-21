# OpenGL Demo - Documentation

欢迎来到OpenGL Demo项目的文档中心！

## 📚 文档结构

### API文档 (`doc/api/`)

**核心模块**
- [Camera API](api/Camera.md) - 相机控制API参考
- [Application API](api/Application.md) - 应用程序主类API参考

**着色器与网格**
- [CShader API](api/CShader.md) - 着色器管理API参考
- [CMesh API](api/CMesh.md) - 网格管理系统API参考
- [CMaterial API](api/CMaterial.md) - 材质系统API参考
- [CTexture API](api/CTexture.md) - 纹理管理API参考
- [CModelLoader API](api/CModelLoader.md) - 模型加载API参考
- [MeshUtils API](api/MeshUtils.md) - 几何体工具API参考

### 架构设计
- [架构设计文档](architecture.md) - 模块结构、类职责、数据流

### 教程 (`doc/tutorial/`)
- [快速入门](tutorial/getting-started.md) - 从零开始使用OpenGL Demo
- [基础网格管理](tutorial/basic-mesh.md) - 创建和管理基础网格
- [材质和纹理](tutorial/materials-textures.md) - 使用材质和纹理系统
- [模型加载](tutorial/model-loading.md) - 从文件加载3D模型
- [着色器进阶](tutorial/advanced-shaders.md) - 高级着色器技术
- [几何体生成](tutorial/geometry-generation.md) - 程序生成复杂几何体

### 示例代码 (`doc/examples/`)
- [基础三角形](examples/basic-triangle.md) - 最简单的三角形示例
- [纹理立方体](examples/textured-cube.md) - 带纹理的立方体
- [模型加载](examples/model-loading.md) - 加载和渲染3D模型
- [多材质渲染](examples/multi-material.md) - 多材质对象渲染
- [实例化渲染](examples/instanced-rendering.md) - 大规模对象渲染

## 🎯 学习路径

1. **初学者** → 快速入门 → 基础三角形
2. **进阶** → 材质和纹理 → 着色器进阶  
3. **高级** → 模型加载 → 几何体生成 → 实例化渲染

## 🔧 快速参考

### 创建网格
```cpp
// 手动创建
std::vector<Vertex> vertices = {...};
CMesh mesh(vertices);
mesh.draw();

// 从文件加载
auto meshes = CModelLoader::load("model.obj");

// 生成几何体
auto cube = MeshUtils::createCube();
```

### 材质设置
```cpp
auto material = std::make_shared<CMaterial>();
material->setColors(glm::vec3(1.0f), glm::vec3(0.5f), glm::vec3(0.1f));
mesh->setMaterial(material);
```

### 着色器使用
```cpp
CShader shader("vs.glsl", "fs.glsl");
shader.use();
shader.setMat4("model", modelMatrix);
```

## 🚀 开始使用

查看 [快速入门](tutorial/getting-started.md) 开始你的OpenGL编程之旅！