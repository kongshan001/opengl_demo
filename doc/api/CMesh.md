# CMesh API 参考

`CMesh` 是OpenGL Demo中的核心网格管理类，负责几何数据的存储、渲染和OpenGL对象管理。

## 类概述

```cpp
class CMesh {
public:
    unsigned int ID;  // OpenGL VAO ID（已弃用，保持兼容性）
    
    // 构造函数
    CMesh();
    CMesh(const std::vector<Vertex>& vertices, PrimitiveType primitive = PrimitiveType::Triangles);
    CMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, PrimitiveType primitive = PrimitiveType::Triangles);
    
    // 拷贝/移动语义
    CMesh(const CMesh& other);
    CMesh& operator=(const Cesh& other);
    CMesh(CMesh&& other) noexcept;
    CMesh& operator=(CMesh&& other) noexcept;
    
    // 析造函数
    ~CMesh();
    
    // 数据管理
    void setVertices(const std::vector<Vertex>& vertices);
    void setIndices(const std::vector<unsigned int>& indices);
    const std::vector<Vertex>& getVertices() const;
    const std::vector<unsigned int>& getIndices() const;
    
    // 顶点属性
    void setVertexLayout(const VertexAttributeLayout& layout);
    const VertexAttributeLayout& getVertexLayout() const;
    
    // 材质管理
    void setMaterial(std::shared_ptr<CMaterial> material);
    std::shared_ptr<CMaterial> getMaterial() const;
    bool hasMaterial() const;
    
    // 渲染接口
    void bind() const;
    void unbind() const;
    void draw() const;
    void drawInstanced(unsigned int instanceCount) const;
    
    // 图元类型
    void setPrimitiveType(PrimitiveType type);
    PrimitiveType getPrimitiveType() const;
    
    // 网格信息
    size_t getVertexCount() const;
    size_t getIndexCount() const;
    bool hasIndices() const;
    
    // 包围盒
    void calculateBoundingBox();
    const BoundingBox& getBoundingBox() const;
    
    // 数据更新
    void updateVertexData(const std::vector<Vertex>& vertices);
    void updateIndexData(const std::vector<unsigned int>& indices);
    
    // 顶点计算
    void calculateNormals();
    void calculateTangentsAndBitangents();
};
```

## 关键方法详解

### 构造函数

#### 默认构造函数
```cpp
CMesh mesh;
```
创建一个空的网格对象，使用默认的顶点布局（PositionNormalTex）。

#### 从顶点数据创建
```cpp
std::vector<Vertex> vertices = {
    Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0, 0, 1.0f), glm::vec2(0.0f, 0.0f)),
    Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0, 0, 1.0f), glm::vec2(1.0f, 0.0f)),
    Vertex(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0, 0, 1.0f), glm::vec2(0.5f, 1.0f))
};

CMesh triangle(vertices);
```

#### 从顶点和索引创建
```cpp
std::vector<Vertex> vertices = {...};  // 8个立方体顶点
std::vector<unsigned int> indices = {
    0, 1, 2,  0, 2, 3,  // 前面
    4, 5, 6,  4, 6, 7   // 后面
    // ... 其他面
};

CMesh cube(vertices, indices);
```

### 数据管理

#### 设置顶点数据
```cpp
// 完全替换顶点数据
mesh.setVertices(newVertices);

// 动态更新（适用于动画网格）
mesh.updateVertexData(newVertices);
```

#### 设置索引数据
```cpp
mesh.setIndices(indices);
mesh.updateIndexData(newIndices);
```

### 顶点属性布局

#### 使用预设布局
```cpp
mesh.setVertexLayout(VertexAttributeLayout::PositionOnly());           // 仅位置
mesh.setVertexLayout(VertexAttributeLayout::PositionNormal());          // 位置 + 法线
mesh.setVertexLayout(VertexAttributeLayout::PositionNormalTex());     // 位置 + 法线 + 纹理坐标
mesh.setVertexLayout(VertexAttributeLayout::Full());                   // 所有属性
```

#### 自定义布局
```cpp
VertexAttributeLayout layout;
layout.addAttribute(VertexAttribute::Position, 3);
layout.addAttribute(VertexAttribute::Normal, 3);
layout.addAttribute(VertexAttribute::TexCoords, 2);
layout.addAttribute(VertexAttribute::Tangent, 3);
mesh.setVertexLayout(layout);
```

### 材质管理

```cpp
auto material = std::make_shared<CMaterial>("RedMaterial");
material->setColors(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f), glm::vec3(0.1f));
material->setProperties(32.0f, 0.8f);

mesh.setMaterial(material);
```

### 渲染

#### 基础渲染
```cpp
mesh.bind();
mesh.draw();
mesh.unbind();
```

#### 实例化渲染
```cpp
// 渲染1000个实例
mesh.drawInstanced(1000);
```

### 包围盒

```cpp
mesh.calculateBoundingBox();
const auto& bbox = mesh.getBoundingBox();

if (bbox.isValid) {
    glm::vec3 center = bbox.getCenter();
    glm::vec3 size = bbox.getSize();
    std::cout << "Bounding box: " << size.x << "x" << size.y << "x" << size.z << std::endl;
}
```

### 顶点计算

#### 计算法线
```cpp
mesh.calculateNormals();
```
自动基于面法线计算每个顶点的法线向量。

#### 计算切线
```cpp
mesh.calculateTangentsAndBitangents();
```
计算用于法线贴图的切线和副切线（需要纹理坐标）。

## 使用示例

### 创建简单三角形
```cpp
#include "mesh/Mesh.h"

// 创建三角形
std::vector<Vertex> triangleVertices = {
    Vertex(glm::vec3( 0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f)),
    Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
    Vertex(glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f))
};

CMesh triangle(triangleVertices);

// 渲染
shader.use();
triangle.draw();
```

### 创建纹理立方体
```cpp
#include "mesh/Mesh.h"
#include "mesh/MeshUtils.h"

// 生成立方体
auto cube = MeshUtils::createCube(2.0f);

// 添加纹理
auto material = std::make_shared<CMaterial>("CubeMaterial");
auto texture = std::make_shared<CTexture>("resources/textures/cube.jpg", TextureType::Diffuse);
material->addTexture(texture);

cube.setMaterial(material);

// 渲染
shader.use();
cube.draw();
```

### 动态网格更新
```cpp
#include "mesh/Mesh.h"

CMesh mesh;
mesh.setVertexLayout(VertexAttributeLayout::PositionNormalTex());

std::vector<Vertex> initialVertices = generateBaseVertices();
mesh.setVertices(initialVertices);

// 在循环中更新顶点（动画）
for (int frame = 0; frame < 100; ++frame) {
    std::vector<Vertex> animatedVertices = animateVertices(initialVertices, frame);
    mesh.updateVertexData(animatedVertices);
    
    renderFrame();
}
```

## 性能注意事项

1. **批量更新**：使用`updateVertexData()`比重新创建网格更高效
2. **实例化渲染**：对于大量相似对象，使用`drawInstanced()`
3. **布局优化**：按顶点属性对齐数据结构
4. **包围盒缓存**：仅在顶点数据改变时重新计算

## 相关类型

### Vertex
```cpp
struct Vertex {
    glm::vec3 position;      // 位置坐标
    glm::vec3 normal;        // 法线向量
    glm::vec2 texCoords;     // 纹理坐标
    glm::vec3 tangent;       // 切线向量
    glm::vec3 bitangent;     // 副切线向量
};
```

### BoundingBox
```cpp
struct BoundingBox {
    glm::vec3 min;           // 最小点
    glm::vec3 max;           // 最大点
    bool isValid;              // 是否有效
    
    glm::vec3 getCenter() const;
    glm::vec3 getSize() const;
    glm::vec3 getRadius() const; // 近似半径
};
```

### PrimitiveType
```cpp
enum class PrimitiveType {
    Triangles = GL_TRIANGLES,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
    Lines = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    Points = GL_POINTS
};
```