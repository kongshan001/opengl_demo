#ifndef MESH_H
#define MESH_H

// Windows compatibility: disable min/max macros
#ifdef _WIN32
    #define NOMINMAX
    #ifdef min
        #undef min
    #endif
    #ifdef max
        #undef max
    #endif
#endif

#include <glad/glad.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "mesh/Vertex.h"
#include "mesh/Material.h"

class CShader;

enum class PrimitiveType {
    Triangles = GL_TRIANGLES,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
    Lines = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    Points = GL_POINTS
};

class CMesh {
public:
    // 构造函数
    CMesh();
    CMesh(const std::vector<Vertex>& vertices, PrimitiveType primitive = PrimitiveType::Triangles);
    CMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, 
           PrimitiveType primitive = PrimitiveType::Triangles);
    
    // 析构函数
    ~CMesh();
    
    // 拷贝/赋值
    CMesh(const CMesh& other);
    CMesh& operator=(const CMesh& other);
    
    // 移动构造/赋值
    CMesh(CMesh&& other) noexcept;
    CMesh& operator=(CMesh&& other) noexcept;
    
    // 顶点数据管理
    void setVertices(const std::vector<Vertex>& vertices);
    void setIndices(const std::vector<unsigned int>& indices);
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
    
    // 顶点属性布局
    void setVertexLayout(const VertexAttributeLayout& layout);
    const VertexAttributeLayout& getVertexLayout() const { return vertexLayout; }
    
    // 材质管理
    void setMaterial(std::shared_ptr<CMaterial> material) { this->material = material; }
    std::shared_ptr<CMaterial> getMaterial() const { return material; }
    bool hasMaterial() const { return material != nullptr; }
    
    // 渲染接口
    void bind() const;
    void unbind() const;
    void draw() const;  // 使用Material中的Shader
    void draw(CShader& shader) const;  // 使用指定Shader
    void drawInstanced(unsigned int instanceCount) const;
    
    // 数据更新
    void updateVertexData(const std::vector<Vertex>& vertices);
    void updateIndexData(const std::vector<unsigned int>& indices);
    
    // 图元类型
    void setPrimitiveType(PrimitiveType type) { primitiveType = type; }
    PrimitiveType getPrimitiveType() const { return primitiveType; }
    
    // 网格信息
    size_t getVertexCount() const { return vertices.size(); }
    size_t getIndexCount() const { return indices.size(); }
    bool hasIndices() const { return !indices.empty(); }
    
    // 包围盒
    struct BoundingBox {
        glm::vec3 min;
        glm::vec3 max;
        bool isValid;
        
        BoundingBox() : min(0.0f), max(0.0f), isValid(false) {}
        BoundingBox(const glm::vec3& minPos, const glm::vec3& maxPos) 
            : min(minPos), max(maxPos), isValid(true) {}
        
        glm::vec3 getCenter() const { return (min + max) * 0.5f; }
        glm::vec3 getSize() const { return max - min; }
        glm::vec3 getRadius() const { return getSize() * 0.5f; }
    };
    
    void calculateBoundingBox();
    const BoundingBox& getBoundingBox() const { return boundingBox; }
    
    // 顶点计算辅助
    void calculateNormals();
    void calculateTangentsAndBitangents();

private:
    // OpenGL对象
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    
    // 数据
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    VertexAttributeLayout vertexLayout;
    
    // 属性
    PrimitiveType primitiveType;
    std::shared_ptr<CMaterial> material;
    BoundingBox boundingBox;
    
    // 是否已初始化
    bool initialized;
    
    // 内部函数
    void initialize();
    void setupVertexAttributes();
    void cleanup();
    
    // 拷贝辅助
    void copyFrom(const CMesh& other);
    
    // 纹理坐标范围计算
    void calculateTextureCoordinateRange();
};

#endif