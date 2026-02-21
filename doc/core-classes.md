# 核心类职责文档

## 概述

本文档描述 OpenGL Demo 项目的核心类及其职责。

---

## 渲染模块

### CShader
**职责**: 着色器程序管理
- 加载、编译、链接着色器
- 管理 uniform 变量
- 缓存 uniform location

**协作**: 被 CMaterial、Application 使用

**生命周期**: RAII 管理，析构时删除 OpenGL 程序

---

### CMesh
**职责**: 网格数据管理
- 存储顶点数据（位置、法线、纹理坐标）
- 管理 VAO/VBO/EBO
- 提供绘制接口

**协作**: 与 CMaterial、CShader 配合绘制

**数据**: Vertex 结构、索引缓冲

---

### CMaterial
**职责**: 材质属性管理
- 管理颜色属性（diffuse、specular、ambient）
- 管理材质参数（shininess、opacity）
- 关联着色器和纹理

**协作**: 被 CMesh 使用，关联 CShader 和 CTexture

---

### CTexture
**职责**: 纹理加载和管理
- 加载图片文件（PNG、JPG）
- 创建 OpenGL 纹理对象
- 管理纹理参数

**协作**: 被 CMaterial 使用

---

### CVertex
**职责**: 顶点数据结构
- 位置、法线、纹理坐标
- 切线、副切线（法线贴图用）

---

## 核心模块

### Camera
**职责**: 3D 相机控制
- FPS 风格视角控制
- 计算视图矩阵、投影矩阵
- 处理键盘/鼠标输入

**协作**: 被 Application 使用

---

### Application
**职责**: 应用程序主类
- 窗口创建（GLFW）
- OpenGL 初始化（GLAD）
- 主循环管理
- 场景渲染

**生命周期**: 单例，RAII 管理

---

## 工具模块

### ModelLoader
**职责**: 模型文件加载
- 解析 OBJ 文件
- 生成 CMesh 对象
- 支持材质组

---

### MeshUtils
**职责**: 几何体生成工具
- 生成基本几何体（立方体、球体等）
- 计算法线、切线

---

## 类关系图

```
Application
    ├── Camera (组合)
    ├── CShader (组合)
    ├── CMaterial (组合)
    │       ├── CShader (关联)
    │       └── CTexture (关联)
    └── CMesh (组合)
            └── CMaterial (关联)

ModelLoader ──创建──► CMesh
MeshUtils ──创建──► CMesh
```

---

## 设计原则

1. **单一职责**: 每个类只负责一个功能
2. **RAII**: 资源在构造函数中获取，析构函数中释放
3. **智能指针**: 使用 shared_ptr 管理共享资源
4. **异常安全**: 错误通过异常抛出，确保资源释放

---

*最后更新: 2026-02-21*
