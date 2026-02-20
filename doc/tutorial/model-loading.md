# 模型加载

本教程将展示如何使用CModelLoader系统从文件加载3D模型。

## 🎯 目标

- 学会OBJ格式的基本结构
- 掌握顶点、法线、纹理坐标等概念
- 学会处理复杂3D模型的加载
- 理解索引化的重要性
- 实践多材质模型的加载和渲染

## 📋 OBJ格式基础

### 1. 文件结构
```obj
# 注释行（以#开头）
# 顶点数据（以v开头）
v -0.5 -0.5 -0.5  # 位置坐标
v 0.5 -0.5 0.0f
v -0.5 0.5 -0.0f
v 0.5 0.5 -0.0f

# 法线数据（以vn开头）
vn 0.0 1.0 0.0f

# 纹理坐标（以vt开头）
vt 0.0 0.0f
vt 0.0 1.0f
vt 0.0 0.0f
vt 0.0 0.0f

# 面数据（以f开头）
f 1/1/1 1 2/3/4 5  # 三个顶点组成一个三角形
f 2/3/4 5/6 7
f 3/1/5 6 7 8
f 1/5/5 6 7 8
```

### 2. 索引语法
OBJ格式使用1-based索引，第一个顶点索引为1：
```obj
v -0.5 -0.5 -0.5 0.0f
v 0.5 -0.5 -0.0f
v 0.5 0.5 0.0f

f 1/1/1 1 2/3/4 5 6 7
f 2/3/4/5/6/7 8
f 1/5/5/6 7 8
```

### 3. 相对面分组
通过`o`或`g`前缀可以分组为材质组：
```obj
# 组1
v 0.0 -0.0 0.0f
v 0.0 -0.0 0.0f
v 0.0 -0.0 0.0f
f 1/1/1 1 1 2/3/4 5/6 7
f 2/3/4/5/6/7 8
s 1 1 1 1 2/3/4/5/6/7 8

# 组2
g 0.0 -0.0 0.0f
g 0.0 -0.0 0.0f
g 0.0 -0.0 0.0f
g 0.0 -0.0 0.0f
g 0.0 -0.0 0.0f
g 0.0 -0.0 0.0f
g 0.0 -0.0 0.0f
g 0.0 -0.0 0.0f
f 0.0 -0.0 0.0f
g 0.0 -0.0 0.0f
f 0.0 -0.0 0.0f
f 0.0 -0.0 0.0f
f 0.0 -0.0 0.0f
f 0.0 -0.0 0.0f
f 1/1/1 1 1 2/3/4 5/6/7 8
f 2/3/4/5/6/7 8
```

## 🎯 加载过程

### 1. 解析流程
```
1. 读取OBJ文件内容
2. 逐行解析顶点、法线、纹理坐标等
3. 构建临时的数据结构
4. 索引化顶点（消除重复）
5. 创建CMesh对象
```

### 2. 关键挑战
- **索引化**：将共享顶点合并以减少GPU内存使用
- **法线计算**：为没有法线的面自动计算顶点法线
- **纹理坐标**：处理不同的纹理坐标系统（可能需要翻转）
- **错误处理**：处理格式错误和缺失数据

## 🛠️ CModelLoader使用

### 1. 简单加载
```cpp
#include "mesh/ModelLoader.h"

// 从文件加载模型
auto meshes = CModelLoader::load("resources/models/car.obj");
```

### 2. 批量处理
```cpp
// 从目录加载所有OBJ文件
std::string modelDir = "resources/models/";
std::vector<std::string> objFiles = {"cube.obj", "sphere.obj", "car.obj"};

for (const auto& file : objFiles) {
    std::string fullPath = modelDir + file;
    auto newMeshes = CModelLoader::load(fullPath);
    meshes.insert(meshes.end(), newMeshes.begin());
}
```

### 3. 错误处理
```cpp
try {
    auto meshes = CModelLoader::load("model.obj");
} catch (const std::exception& e) {
    std::cout << "Failed to load model: " << e.what() << std::endl;
    return {};
}
```

## 🎯 纹理示例

### 1. 创建单个网格对象
```cpp
auto mesh = CModelLoader::load("resources/models/character.obj");
mesh->draw();
```

### 2. 创建多材质对象
```cpp
auto meshes = CModelLoader::load("resources/models/car.obj");

// 为每个网格设置不同材质
for (size_t i = 0; i < meshes.size(); ++i) {
    auto material = std::make_shared<CMaterial>("Object" + std::to_string(i));
    
    // 设置不同颜色
    switch (i) {
        case 0: material->setColors(glm::vec3(0.8f, 0.2f, 0.1f)); break;
        case 1: material->setColors(glm::vec3(0.9f, 0.1f, 0.1f)); break;
        case 2: material->setColors(glm::vec3(0.0f, 1.0f, 0.0f)); break;
    }
    
    meshes[i]->setMaterial(material);
}
```

### 3. 动态加载多个模型
```cpp
class ModelManager {
public:
    std::vector<std::string> modelPaths;
    std::vector<std::vector<std::shared_ptr<CMesh>>> models;
    
    void loadAllModels(const std::string& directory);
    void renderAllModels();
    void renderModel(size_t index);
private:
    std::map<std::string, std::vector<std::shared_ptr<CMesh>>> modelMap;
};
```

## 📊 高级用法

### 1. 材质分组
```cpp
// 加载模型并按材质分组
auto meshes = CModelLoader::load("scene.obj");
std::map<std::string, std::vector<std::shared_ptr<CMesh>>> materialMeshes;

// 按材质分组
for (const auto& mesh : meshes) {
    std::string materialType = mesh->getMaterial() ? mesh->getMaterial()->getName() : "Default";
    if (materialMeshes.find(materialType) == materialMeshes.end()) {
        materialMeshes[materialType] = {};
    }
    materialMeshes[materialType].push_back(mesh);
}

// 批染相同材质的对象
for (const auto& pair : materialMeshes) {
    for (const auto& mesh : pair.second) {
        mesh->draw();
    }
}
```

### 2. 场景组织
```cpp
struct Scene {
    std::vector<std::shared_ptr<CMesh>> allMeshes;
    std::vector<std::shared_ptr<CMaterial>> materials;
    
    void addMesh(std::shared_ptr<CMesh> mesh) {
        allMeshes.push_back(mesh);
        if (mesh->hasMaterial()) {
            materials.push_back(mesh->getMaterial());
        }
    }
};
```

## 🔧 性能优化

### 1. 大模型加载
```cpp
// 对于非常大的模型，考虑分块加载
class StreamingModelLoader {
    std::vector<std::shared_ptr<CMesh>> loadLargeModel(const std::string& filepath, size_t chunkSize = 1000);
};
```

### 2. 异步加载
```cpp
std::future<std::vector<std::shared_ptr<CMesh>> loadModelAsync(const std::string& filepath);
```

### 3. 内存池管理
```cpp
class MeshPool {
    std::queue<std::shared_ptr<CMesh>> availableMeshes;
    std::vector<std::shared_ptr<CMesh>> usedMeshes;
    
    std::shared_ptr<CMesh> getMesh();
    void returnMesh(std::shared_ptr<CMesh> mesh);
};
```

## 🧰 调试和调试

### 1. 模型验证
```cpp
// 检查模型是否有效
bool validateModel(const CMesh& mesh) {
    return mesh.getVertexCount() > 0 && 
           (!mesh.hasIndices() || mesh.getIndexCount() > 0);
}

// 打印模型统计信息
void printModelInfo(const CMesh& mesh) {
    std::cout << "Mesh Info:" << std::endl;
    std::cout << "  Vertices: " << mesh.getVertexCount() << std::endl;
    std::cout << "  Indices: " << mesh.getIndexCount() << std::endl;
    std::cout << "  Bounding Box: " 
              << "Min: (" << bbox.min.x << ", " << bbox.min.y << ", " << bbox.min.z << ")" << std::endl;
    std::cout << "  Max: (" << bbox.max.x << ", " << bbox.max.y << ", " << bbox.max.z << ")" << std::endl;
}
```

### 2. 错误诊断
```cpp
// 检查文件是否存在
bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

// 检查文件格式
bool isValidOBJ(const std::string& path) {
    return path.substr(path.find_last_of('.') + 1) == "obj";
}
```

### 3. 内容查看
```cpp
// 快速查看文件开头
std::string previewFile(const std::string& path, size_t lines = 10) {
    std::ifstream file(path);
    std::string content;
    std::string line;
    for (size_t i = 0; i < lines && std::getline(file, line); ++i) {
        content += line + "\n";
        if (i == lines) content += "...";
    }
    std::cout << "File preview (" << lines << " lines):\n" << content << std::endl;
}
```

## 🚀 示例项目

### 1. 场景加载器
创建一个场景管理器，可以：
```cpp
class SceneManager {
    std::vector<std::shared_ptr<CMesh>> meshes;
    std::vector<Light> lights;
    
    void loadScene(const std::string& sceneFile);
    void renderScene();
    void addMesh(std::shared_ptr<CMesh> mesh);
    void addLight(const Light& light);
    
    void setActiveCamera(const Camera& camera);
    void setViewProjection(const glm::mat4& view, const glm::mat4& projection);
};
```

### 2. 程览器
创建3D场景查看器：
```cpp
class ModelViewer {
    std::vector<std::shared_ptr<CMesh>> meshes;
    Camera camera;
    
    void loadModel(const std::string& path);
    void render();
    void setView(const glm::vec3& position, const glm::vec3& target);
};
```

### 3. 动画控制器
添加鼠标和键盘控制用于：
- 相机旋转（轨道模式）
- 网格浏览（WASD键控制）
- 对象选择和操作

现在你已经掌握了OpenGL Demo的完整网格管理系统！从简单的三角形到复杂的3D模型，从基础纹理到PBR材质，你现在具备了创建现代OpenGL应用所需的全部工具。继续扩展你的3D世界吧！