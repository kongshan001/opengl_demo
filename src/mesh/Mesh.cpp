#include "mesh/Mesh.h"
#include "shader/Shader.h"
#include <iostream>

CMesh::CMesh() 
    : VAO(0), VBO(0), EBO(0), 
      primitiveType(PrimitiveType::Triangles),
      material(nullptr),
      initialized(false) {
    vertexLayout = VertexAttributeLayout::PositionNormalTex();
}

CMesh::CMesh(const std::vector<Vertex>& vertices, PrimitiveType primitive)
    : VAO(0), VBO(0), EBO(0),
      vertices(vertices), indices(),
      primitiveType(primitive),
      material(nullptr),
      initialized(false) {
    vertexLayout = VertexAttributeLayout::PositionNormalTex();
    initialize();
}

CMesh::CMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, PrimitiveType primitive)
    : VAO(0), VBO(0), EBO(0),
      vertices(vertices), indices(indices),
      primitiveType(primitive),
      material(nullptr),
      initialized(false) {
    vertexLayout = VertexAttributeLayout::PositionNormalTex();
    initialize();
}

CMesh::~CMesh() {
    cleanup();
}

CMesh::CMesh(const CMesh& other) 
    : VAO(0), VBO(0), EBO(0),
      vertices(other.vertices),
      indices(other.indices),
      vertexLayout(other.vertexLayout),
      primitiveType(other.primitiveType),
      material(other.material),
      boundingBox(other.boundingBox),
      initialized(false) {
    initialize();
}

CMesh& CMesh::operator=(const CMesh& other) {
    if (this != &other) {
        cleanup();
        
        vertices = other.vertices;
        indices = other.indices;
        vertexLayout = other.vertexLayout;
        primitiveType = other.primitiveType;
        material = other.material;
        boundingBox = other.boundingBox;
        initialized = false;
        
        initialize();
    }
    return *this;
}

CMesh::CMesh(CMesh&& other) noexcept 
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
      vertices(std::move(other.vertices)),
      indices(std::move(other.indices)),
      vertexLayout(other.vertexLayout),
      primitiveType(other.primitiveType),
      material(other.material),
      boundingBox(other.boundingBox),
      initialized(other.initialized) {
    
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.initialized = false;
}

CMesh& CMesh::operator=(CMesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        vertexLayout = other.vertexLayout;
        primitiveType = other.primitiveType;
        material = other.material;
        boundingBox = other.boundingBox;
        initialized = other.initialized;
        
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.initialized = false;
    }
    return *this;
}

void CMesh::setVertices(const std::vector<Vertex>& newVertices) {
    vertices = newVertices;
    if (!initialized) {
        initialize();
    } else {
        updateVertexData(newVertices);
    }
}

void CMesh::setIndices(const std::vector<unsigned int>& newIndices) {
    indices = newIndices;
    if (!initialized) {
        initialize();
    } else {
        updateIndexData(newIndices);
    }
}

void CMesh::setVertexLayout(const VertexAttributeLayout& layout) {
    vertexLayout = layout;
    if (initialized) {
        setupVertexAttributes();
    }
}

void CMesh::bind() const {
    if (initialized) {
        glBindVertexArray(VAO);
    }
}

void CMesh::unbind() const {
    glBindVertexArray(0);
}

void CMesh::draw() const {
    if (!initialized || vertices.empty()) return;
    
    // 如果有材质且材质有shader，使用材质的shader
    if (material && material->hasShader()) {
        material->apply();  // 使用内置shader并应用材质参数
    }
    
    bind();
    
    if (hasIndices()) {
        glDrawElements(static_cast<GLenum>(primitiveType), static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(static_cast<GLenum>(primitiveType), 0, static_cast<GLsizei>(vertices.size()));
    }
    
    unbind();
}

void CMesh::draw(CShader& shader) const {
    if (!initialized || vertices.empty()) return;
    
    // 使用指定的shader
    shader.use();
    
    // 如果有材质，应用材质参数到shader
    if (material) {
        material->applyToShader(shader);
    }
    
    bind();
    
    if (hasIndices()) {
        glDrawElements(static_cast<GLenum>(primitiveType), static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(static_cast<GLenum>(primitiveType), 0, static_cast<GLsizei>(vertices.size()));
    }
    
    unbind();
}

void CMesh::drawInstanced(unsigned int instanceCount) const {
    if (!initialized || vertices.empty()) return;
    
    bind();
    
    if (hasIndices()) {
        glDrawElementsInstanced(static_cast<GLenum>(primitiveType), static_cast<GLsizei>(indices.size()), 
                              GL_UNSIGNED_INT, 0, instanceCount);
    } else {
        glDrawArraysInstanced(static_cast<GLenum>(primitiveType), 0, static_cast<GLsizei>(vertices.size()), instanceCount);
    }
    
    unbind();
}

void CMesh::updateVertexData(const std::vector<Vertex>& newVertices) {
    vertices = newVertices;
    
    if (initialized) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void CMesh::updateIndexData(const std::vector<unsigned int>& newIndices) {
    indices = newIndices;
    
    if (initialized && hasIndices()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void CMesh::calculateBoundingBox() {
    if (vertices.empty()) {
        boundingBox = BoundingBox();
        return;
    }
    
    glm::vec3 minPos = vertices[0].position;
    glm::vec3 maxPos = vertices[0].position;
    
    for (const auto& vertex : vertices) {
        minPos = glm::min(minPos, vertex.position);
        maxPos = glm::max(maxPos, vertex.position);
    }
    
    boundingBox = BoundingBox(minPos, maxPos);
}

void CMesh::calculateNormals() {
    // 简单的法线计算 - 为每个面计算法线
    if (hasIndices()) {
        // 基于索引的法线计算
        for (size_t i = 0; i < indices.size(); i += 3) {
            if (i + 2 >= indices.size()) break;
            
            const Vertex& v0 = vertices[indices[i]];
            const Vertex& v1 = vertices[indices[i + 1]];
            const Vertex& v2 = vertices[indices[i + 2]];
            
            glm::vec3 edge1 = v1.position - v0.position;
            glm::vec3 edge2 = v2.position - v0.position;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            
            // 将法线应用到三个顶点
            const_cast<Vertex&>(v0).normal = normal;
            const_cast<Vertex&>(v1).normal = normal;
            const_cast<Vertex&>(v2).normal = normal;
        }
    } else {
        // 基于顶点的法线计算
        for (size_t i = 0; i < vertices.size(); i += 3) {
            if (i + 2 >= vertices.size()) break;
            
            Vertex& v0 = vertices[i];
            Vertex& v1 = vertices[i + 1];
            Vertex& v2 = vertices[i + 2];
            
            glm::vec3 edge1 = v1.position - v0.position;
            glm::vec3 edge2 = v2.position - v0.position;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            
            v0.normal = normal;
            v1.normal = normal;
            v2.normal = normal;
        }
    }
}

void CMesh::calculateTangentsAndBitangents() {
    // TODO: 实现切线和副切线计算
    // 这需要基于纹理坐标和位置来计算
    std::cout << "Tangent calculation not yet implemented" << std::endl;
}

void CMesh::initialize() {
    if (initialized) return;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    if (!vertices.empty()) {
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        
        if (hasIndices()) {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        }
        
        setupVertexAttributes();
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (hasIndices()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
    
    calculateBoundingBox();
    initialized = true;
}

void CMesh::setupVertexAttributes() {
    for (const auto& attr : vertexLayout.attributes) {
        switch (attr.type) {
            case VertexAttribute::Position:
                glVertexAttribPointer(static_cast<GLuint>(attr.type), attr.count, GL_FLOAT, GL_FALSE, vertexLayout.stride, (void*)attr.offset);
                glEnableVertexAttribArray(static_cast<GLuint>(attr.type));
                break;
            case VertexAttribute::Normal:
                glVertexAttribPointer(static_cast<GLuint>(attr.type), attr.count, GL_FLOAT, GL_FALSE, vertexLayout.stride, (void*)attr.offset);
                glEnableVertexAttribArray(static_cast<GLuint>(attr.type));
                break;
            case VertexAttribute::TexCoords:
                glVertexAttribPointer(static_cast<GLuint>(attr.type), attr.count, GL_FLOAT, GL_FALSE, vertexLayout.stride, (void*)attr.offset);
                glEnableVertexAttribArray(static_cast<GLuint>(attr.type));
                break;
            case VertexAttribute::Tangent:
                glVertexAttribPointer(static_cast<GLuint>(attr.type), attr.count, GL_FLOAT, GL_FALSE, vertexLayout.stride, (void*)attr.offset);
                glEnableVertexAttribArray(static_cast<GLuint>(attr.type));
                break;
            case VertexAttribute::Bitangent:
                glVertexAttribPointer(static_cast<GLuint>(attr.type), attr.count, GL_FLOAT, GL_FALSE, vertexLayout.stride, (void*)attr.offset);
                glEnableVertexAttribArray(static_cast<GLuint>(attr.type));
                break;
            default:
                break;
        }
    }
}

void CMesh::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    
    initialized = false;
}

void CMesh::calculateTextureCoordinateRange() {
    // TODO: 实现纹理坐标范围计算
}