# ModelLoader API 参考

模型加载系统，支持多种 3D 模型格式的加载。

## 异常类

```cpp
class ModelLoadException : public std::runtime_error {
public:
    explicit ModelLoadException(const std::string& message);
};
```

当以下情况发生时抛出 `ModelLoadException`：
- 模型文件无法打开
- 模型文件格式不支持
- 模型数据无效（如无顶点）
- 索引越界

## 核心类

### IModelLoader - 加载器接口

```cpp
class IModelLoader {
public:
    virtual ~IModelLoader() = default;
    
    // 加载模型，返回网格列表
    virtual std::vector<std::shared_ptr<CMesh>> loadModel(const std::string& filepath) = 0;
    
    // 检查是否支持该格式
    virtual bool canLoad(const std::string& filepath) const = 0;
    
    // 获取支持的扩展名
    virtual const char* getSupportedExtension() const = 0;
};
```

### CModelLoader - 简化接口

```cpp
class CModelLoader {
public:
    // 加载模型文件（失败时抛出 ModelLoadException）
    static std::vector<std::shared_ptr<CMesh>> load(const std::string& filepath);
    
    // 检查文件格式是否支持
    static bool isSupported(const std::string& filepath);
    
    // 获取支持的格式列表
    static std::vector<std::string> getSupportedFormats();
    
    // 禁止实例化
    CModelLoader() = delete;
};
```

## 使用示例

### 基本加载

```cpp
#include "mesh/ModelLoader.h"

try {
    // 加载 OBJ 模型
    auto meshes = CModelLoader::load("resources/models/cube.obj");
    
    // 渲染所有网格
    CShader shader("vertex.glsl", "fragment.glsl");
    shader.use();
    
    for (const auto& mesh : meshes) {
        mesh->draw(shader);
    }
} catch (const ModelLoadException& e) {
    std::cerr << "Failed to load model: " << e.what() << std::endl;
}
```

### 检查格式支持

```cpp
if (CModelLoader::isSupported("model.obj")) {
    auto meshes = CModelLoader::load("model.obj");
}

// 获取所有支持的格式
for (const auto& format : CModelLoader::getSupportedFormats()) {
    std::cout << "Supported: " << format << std::endl;
}
// 输出: Supported: obj
```

### 使用工厂创建加载器

```cpp
// 获取特定格式的加载器
auto loader = ModelLoaderFactory::createLoader("model.obj");

if (loader) {
    // loader 是 std::unique_ptr<IModelLoader>
    if (loader->canLoad("model.obj")) {
        auto meshes = loader->loadModel("model.obj");
    }
}
```

## 支持的格式

| 格式 | 扩展名 | 特性支持 |
|------|--------|---------|
| OBJ  | `.obj` | 顶点、法线、纹理坐标、面 |

## OBJ 加载器特性

### 自动处理

- **索引化顶点**：消除重复顶点，优化内存
- **法线计算**：如果模型无法线，自动计算
- **包围盒计算**：预计算用于视锥剔除
- **边界检查**：防止越界访问

### 支持的 OBJ 语法

```obj
# 顶点
v 0.0 0.0 0.0

# 法线
vn 0.0 1.0 0.0

# 纹理坐标
vt 0.5 0.5

# 面（多种格式）
f 1 2 3                    # 仅位置
f 1/1 2/2 3/3             # 位置/纹理
f 1//1 2//2 3//3          # 位置//法线
f 1/1/1 2/2/2 3/3/3       # 位置/纹理/法线
```

## 错误处理

### 常见错误

| 错误类型 | 信息示例 |
|---------|---------|
| 文件未找到 | `Failed to open OBJ file: missing.obj` |
| 格式不支持 | `Unsupported model format: model.xyz` |
| 无顶点 | `OBJ file contains no vertices: empty.obj` |
| 索引越界 | `Invalid position index in OBJ file` |

### 异常捕获

```cpp
try {
    auto meshes = CModelLoader::load(filepath);
} catch (const ModelLoadException& e) {
    // 处理加载错误
    logError(e.what());
} catch (const std::exception& e) {
    // 其他异常
    logError(std::string("Unexpected error: ") + e.what());
}
```

## 内部实现

### OBJLoader 类

```cpp
class OBJLoader : public IModelLoader {
public:
    std::vector<std::shared_ptr<CMesh>> loadModel(const std::string& filepath) override;
    bool canLoad(const std::string& filepath) const override;
    const char* getSupportedExtension() const override { return "obj"; }

private:
    struct OBJIndex {
        unsigned int positionIndex;
        unsigned int normalIndex;
        unsigned int texCoordIndex;
    };
    
    void parseOBJ(...);
    void createMesh(...);
};
```

### ModelLoaderFactory 类

```cpp
class ModelLoaderFactory {
public:
    // 创建加载器（返回 unique_ptr，RAII）
    static std::unique_ptr<IModelLoader> createLoader(const std::string& filepath);
    
    // 注册新的加载器
    static void registerLoader(std::unique_ptr<IModelLoader> loader);
};
```

## 扩展新格式

### 1. 实现加载器

```cpp
class FBXLoader : public IModelLoader {
public:
    std::vector<std::shared_ptr<CMesh>> loadModel(const std::string& filepath) override {
        // 解析 FBX 文件...
    }
    
    bool canLoad(const std::string& filepath) const override {
        return endsWith(filepath, ".fbx");
    }
    
    const char* getSupportedExtension() const override { return "fbx"; }
};
```

### 2. 注册加载器

```cpp
ModelLoaderFactory::registerLoader(
    std::unique_ptr<IModelLoader>(new FBXLoader())
);
```

## 性能考虑

- **顶点去重**：使用哈希表消除重复顶点
- **智能指针**：返回 `shared_ptr<CMesh>` 便于共享
- **延迟加载**：仅在需要时加载资源

## 最佳实践

1. **检查文件存在**
```cpp
if (!std::filesystem::exists(filepath)) {
    // 处理缺失文件
}
```

2. **使用异常处理**
```cpp
try {
    auto meshes = CModelLoader::load(filepath);
    if (meshes.empty()) {
        // 空模型处理
    }
} catch (const ModelLoadException& e) {
    // 错误处理
}
```

3. **复用网格**
```cpp
// 一次加载，多次渲染
static auto meshes = CModelLoader::load("common.obj");
for (const auto& mesh : meshes) {
    mesh->draw(shader);
}
```
