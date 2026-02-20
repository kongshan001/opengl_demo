# 项目迭代日志

本文档记录项目的持续迭代过程，包括时间、问题背景、原因分析和解决方案。

---

## 2026-02-20

### Material-Shader 集成重构

**问题**：材质和着色器分离，渲染时需要外部管理 shader 状态，不符合商业引擎设计模式。

**背景**：
- 原 `CMaterial::apply(CShader& shader)` 需要外部传入 shader
- 渲染流程：外部设置 shader → 外部设置材质 → 绘制
- Unity/Unreal 等引擎中 Material 持有 Shader 引用

**原因**：
- 设计初期未考虑完整的渲染管线架构
- 缺乏对商业引擎设计模式的研究

**方案**：
```cpp
// Material 现在持有 shader
class CMaterial {
    std::shared_ptr<CShader> shader;
public:
    void setShader(std::shared_ptr<CShader> shader);
    void apply() const;  // 使用内置 shader
    void applyToShader(CShader& shader) const;  // 兼容接口
};

// CMesh::draw() 自动使用 Material 的 shader
void CMesh::draw() const {
    if (material && material->hasShader()) {
        material->apply();
    }
    // ... 绑定并绘制
}
```

**效果**：
- 内聚性提升：一个 Material 对象完整描述渲染状态
- 易用性提升：`mesh.setMaterial(material); mesh.draw();` 即可
- Shader 仍可被多个 Material 共享（shared_ptr）

---

### Google Test 单元测试框架集成

**问题**：项目缺乏自动化测试，代码质量保障不足。

**背景**：
- 项目规模逐渐增大（CMesh, CMaterial, CTexture, CModelLoader 等模块）
- 每次修改后需要手动验证
- 缺乏回归测试能力

**原因**：
- 初期为快速原型开发，未考虑测试
- C++ 项目测试框架选型未确定

**方案**：
- 集成 Google Test 1.14.0
- 创建 `tests/` 目录存放测试文件
- CMake 集成 CTest 自动发现测试
- 编写 Vertex 和 Material 的单元测试（20 个测试用例）

```bash
# 运行测试
cd build && ./opengl_tests

# 运行特定测试
./opengl_tests --gtest_filter=MaterialTest.*
```

**效果**：
- 20 个测试全部通过
- 可持续集成支持
- 回归测试能力

---

### CMake 模块化重构

**问题**：CMakeLists.txt 文件逐渐变大，职责不清晰。

**背景**：
- 原 CMakeLists.txt 约 80 行
- 依赖配置、平台配置、测试配置混在一起
- 难以维护和扩展

**原因**：
- 初期项目简单，单一文件足够
- 随功能增加未及时重构

**方案**：
```
cmake/
├── Dependencies.cmake   # OpenGL 查找、包含目录、源文件收集
├── Platform.cmake       # 平台特定链接库配置
└── Testing.cmake        # Google Test 集成

CMakeLists.txt           # 主配置文件（约 50 行）
```

```cmake
# CMakeLists.txt 核心结构
include(Dependencies)
include(Platform)
# ... 主可执行文件配置 ...
include(Testing)
```

**效果**：
- 职责分离，易于维护
- 新增平台/依赖只需修改对应模块
- 主配置文件简洁清晰

---

## 2026-02-10

### CMesh 模块化系统构建

**问题**：原有代码仅有简单三角形渲染，无法支持复杂 3D 模型。

**背景**：
- 原项目结构简单（仅 CShader 和 main.cpp）
- 缺乏网格、材质、纹理等核心模块
- 无法加载外部模型文件

**原因**：
- 项目定位从简单 demo 转向完整渲染框架
- 需要支持更复杂的渲染场景

**方案**：
- 创建 5 个核心模块：CMesh, CTexture, CMaterial, CModelLoader, MeshUtils
- 添加 GLM 数学库和 stb_image 纹理库
- 实现 OBJ 格式模型加载
- 创建 resources/ 目录管理着色器、纹理、模型

```
include/mesh/
├── Vertex.h
├── Mesh.h
├── Material.h
├── Texture.h
├── ModelLoader.h
└── MeshUtils.h
```

**效果**：
- 完整的网格管理能力
- 支持 OBJ 模型加载
- 可扩展的材质/纹理系统

---

## 2026-02-04

### Python 版本移除

**问题**：同时维护 Python 和 C++ 两个版本，精力分散。

**背景**：
- 项目最初有 Python (PyGame + PyOpenGL) 和 C++ 两个实现
- Python 版本使用 OpenGL 1.2 旧版着色器
- C++ 版本使用 OpenGL 3.3 Core Profile 现代管线

**原因**：
- 目标是学习现代 OpenGL，C++ 版本更合适
- 维护两个版本增加工作量
- Python 版本功能受限

**方案**：
- 删除 `opengl_demo.py` 和 `requirements.txt`
- 专注于 C++ 版本开发
- 更新文档反映新架构

**效果**：
- 开发精力集中
- 代码库更清晰
- 专注于现代 OpenGL

---

## 2026-02-01

### 项目初始化

**问题**：需要学习现代 OpenGL 渲染技术。

**背景**：
- 学习 OpenGL 的需求
- 需要一个可编译运行的起点项目

**原因**：
- 系统学习图形编程
- 理解渲染管线

**方案**：
- 创建基础项目结构
- 集成 GLFW (窗口管理) 和 GLAD (OpenGL 加载器)
- 实现简单三角形渲染
- 同时提供 Python 和 C++ 两个版本

**效果**：
- 可编译运行的 OpenGL 3.3 Core Profile 项目
- 跨平台支持 (macOS/Linux/Windows)
- 清晰的项目结构

---

## 迭代原则

1. **渐进式改进**：每次迭代解决一个明确的问题
2. **保持向后兼容**：添加兼容接口，不破坏现有代码
3. **文档同步**：每次迭代更新相关文档
4. **测试保障**：新功能添加对应单元测试
5. **模块化设计**：职责分离，易于维护和扩展

---

## 未来计划

| 优先级 | 功能 | 状态 |
|--------|------|------|
| 高 | 鼠标控制相机 | 待开发 |
| 高 | 修复 Mesh.cpp 指针转换警告 | 待处理 |
| 中 | 高级光照着色器 (Phong/Blinn-Phong) | 待开发 |
| 中 | 纹理加载和渲染示例 | 待开发 |
| 低 | 更多模型格式支持 (glTF) | 待评估 |
| 低 | 更多单元测试覆盖 | 待开发 |
