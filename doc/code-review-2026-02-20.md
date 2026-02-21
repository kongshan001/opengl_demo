# 代码质量分析报告

**日期**: 2026-02-20
**分析工具**: OpenCode (GLM-5)

## 统计摘要

| 严重程度 | 数量 |
|---------|-----|
| 🔴 Critical | 4 |
| 🟠 Major | 7 |
| 🟡 Minor | 5 |
| **总计** | **16** |

---

## 🔴 Critical (严重)

### C1. int-to-pointer-cast 警告 - 不安全的类型转换
**文件**: `src/mesh/Mesh.cpp:314, 318, 322, 326, 330`

```cpp
// 问题代码
glVertexAttribPointer(static_cast<GLuint>(attr.type), attr.count, GL_FALSE,
                      vertexLayout.stride, (void*)attr.offset);
```

**问题**: 将 `unsigned int` 直接强转为 `void*` 在 64 位系统上可能导致数据截断和未定义行为。

**修复建议**:
```cpp
glVertexAttribPointer(static_cast<GLuint>(attr.type), attr.count, GL_FALSE,
                      vertexLayout.stride,
                      reinterpret_cast<void*>(static_cast<uintptr_t>(attr.offset)));
```

---

### C2. CShader 析构时可能删除无效 ID
**文件**: `src/shader/Shader.cpp:22-24`

```cpp
// 问题代码
CShader::~CShader() {
    glDeleteProgram(ID);
}
```

**问题**: 如果 `ID` 为 0（编译/链接失败时），调用 `glDeleteProgram` 是未定义行为。

**修复建议**:
```cpp
CShader::~CShader() {
    if (ID != 0) {
        glDeleteProgram(ID);
    }
}
```

---

### C3. CShader 缺少移动语义 - 资源双重释放
**文件**: `include/shader/Shader.h:55`

**问题**: `CShader` 类缺少移动构造/移动赋值运算符。当 `CShader` 对象被移动后，原对象的 `ID` 仍保持不变，导致析构时双重删除 OpenGL 程序。

**修复建议**: 添加移动语义:
```cpp
CShader(CShader&& other) noexcept : ID(other.ID), uniformLocationCache(std::move(other.uniformLocationCache)) {
    other.ID = 0;
}

CShader& operator=(CShader&& other) noexcept {
    if (this != &other) {
        if (ID != 0) glDeleteProgram(ID);
        ID = other.ID;
        uniformLocationCache = std::move(other.uniformLocationCache);
        other.ID = 0;
    }
    return *this;
}

// 禁止拷贝
CShader(const CShader&) = delete;
CShader& operator=(const CShader&) = delete;
```

---

### C4. 静态成员变量未初始化
**文件**: `src/mesh/ModelLoader.cpp:159`

```cpp
std::vector<std::unique_ptr<IModelLoader>> ModelLoaderFactory::loaders;
```

**问题**: 静态成员变量 `loaders` 未被初始化，且没有清理机制。

**修复建议**: 添加清理函数或使用单例模式。

---

## 🟠 Major (重要)

### M1. const_cast 滥用
**文件**: `src/mesh/Mesh.cpp:249-251`

```cpp
const_cast<Vertex&>(v0).normal = normal;
```

**问题**: `const_cast` 表明设计问题。

**修复建议**: 重新设计方法签名，使用非 const 引用或返回新 vector。

---

### M2. setVertices/setIndices 不必要的拷贝
**文件**: `src/mesh/Mesh.cpp:105-121`

**修复建议**: 添加右值引用重载:
```cpp
void setVertices(std::vector<Vertex>&& newVertices);
void setIndices(std::vector<unsigned int>&& newIndices);
```

---

### M3. 着色器链接失败后资源泄漏风险
**文件**: `src/shader/Shader.cpp:89-115`

**修复建议**: 使用 RAII wrapper 管理 shader objects。

---

### M4. 纹理加载失败未抛出异常
**文件**: `src/mesh/Texture.cpp:17-19`

```cpp
std::cout << "Failed to load texture: " << filepath << std::endl;
```

**修复建议**: 抛出 `TextureLoadException` 或返回 `std::optional<CTexture>`。

---

### M5. applyToShader 方法语义矛盾
**文件**: `include/mesh/Material.h:61`

```cpp
void applyToShader(CShader& shader) const;  // const 方法但修改 shader
```

**问题**: 方法标记为 `const` 但会调用 shader 的 set 方法。

---

### M6. CMaterial 拷贝构造函数冗余代码
**文件**: `src/mesh/Material.cpp:30-69`

**修复建议**: 使用 copy-and-swap 惯用法。

---

### M7. 全局变量污染
**文件**: `src/main.cpp:13-18`

**修复建议**: 封装到 `Camera` 类中。

---

## 🟡 Minor (次要)

| 问题 | 文件 | 建议 |
|-----|------|------|
| m1 | src/main.cpp:10-11 | 魔数可配置化 |
| m2 | include/mesh/Mesh.h:44-45 | 简单 getter 应标记 noexcept |
| m3 | include/mesh/Mesh.h:123 | 未实现的私有方法 copyFrom |
| m4 | src/mesh/Mesh.cpp:273-277 | 空实现 calculateTangentsAndBitangents |
| m5 | include/mesh/Material.h:55 | 默认参数一致性 |

---

## 📊 测试覆盖分析

| 模块 | 测试文件 | 覆盖情况 |
|------|---------|---------|
| CShader | test_shader.cpp | ⚠️ 需要上下文 |
| CMesh | - | ❌ 无测试 |
| CMaterial | test_material.cpp | ✅ 基础测试 |
| CTexture | - | ❌ 无测试 |
| CModelLoader | - | ❌ 无测试 |
| Vertex | test_vertex.cpp | ✅ 良好 |

---

## 优先修复建议

1. **C1**: 修复 int-to-pointer-cast（编译警告）
2. **C2**: CShader 析构函数检查 ID
3. **C3**: 添加 CShader 移动语义
4. **M4**: CTexture 添加异常处理

---

## 下一步行动

- [ ] 修复 Critical 问题 (C1-C4)
- [ ] 修复 Major 问题 (M1-M7)
- [ ] 添加 CMesh 单元测试
- [ ] 添加 CTexture 单元测试
- [ ] 添加 CModelLoader 单元测试
