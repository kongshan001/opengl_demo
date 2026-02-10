#include "mesh/MeshUtils.h"
#include <cmath>
#include <iostream>

// 基础几何体生成
std::shared_ptr<CMesh> MeshUtils::createCube(float size) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float halfSize = size * 0.5f;
    
    // 立方体的8个顶点
    vertices = {
        // 前面 (Z+)
        Vertex(glm::vec3(-halfSize, -halfSize, halfSize), glm::vec3(0, 0, 1), glm::vec2(0, 0)),
        Vertex(glm::vec3(halfSize, -halfSize, halfSize), glm::vec3(0, 0, 1), glm::vec2(1, 0)),
        Vertex(glm::vec3(halfSize, halfSize, halfSize), glm::vec3(0, 0, 1), glm::vec2(1, 1)),
        Vertex(glm::vec3(-halfSize, halfSize, halfSize), glm::vec3(0, 0, 1), glm::vec2(0, 1)),
        
        // 后面 (Z-)
        Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0, 0, -1), glm::vec2(0, 0)),
        Vertex(glm::vec3(halfSize, -halfSize, -halfSize), glm::vec3(0, 0, -1), glm::vec2(1, 0)),
        Vertex(glm::vec3(halfSize, halfSize, -halfSize), glm::vec3(0, 0, -1), glm::vec2(1, 1)),
        Vertex(glm::vec3(-halfSize, halfSize, -halfSize), glm::vec3(0, 0, -1), glm::vec2(0, 1)),
        
        // 左面 (X-)
        Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(-1, 0, 0), glm::vec2(0, 0)),
        Vertex(glm::vec3(-halfSize, -halfSize, halfSize), glm::vec3(-1, 0, 0), glm::vec2(1, 0)),
        Vertex(glm::vec3(-halfSize, halfSize, halfSize), glm::vec3(-1, 0, 0), glm::vec2(1, 1)),
        Vertex(glm::vec3(-halfSize, halfSize, -halfSize), glm::vec3(-1, 0, 0), glm::vec2(0, 1)),
        
        // 右面 (X+)
        Vertex(glm::vec3(halfSize, -halfSize, -halfSize), glm::vec3(1, 0, 0), glm::vec2(0, 0)),
        Vertex(glm::vec3(halfSize, -halfSize, halfSize), glm::vec3(1, 0, 0), glm::vec2(1, 0)),
        Vertex(glm::vec3(halfSize, halfSize, halfSize), glm::vec3(1, 0, 0), glm::vec2(1, 1)),
        Vertex(glm::vec3(halfSize, halfSize, -halfSize), glm::vec3(1, 0, 0), glm::vec2(0, 1)),
        
        // 底面 (Y-)
        Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0, -1, 0), glm::vec2(0, 0)),
        Vertex(glm::vec3(halfSize, -halfSize, -halfSize), glm::vec3(0, -1, 0), glm::vec2(1, 0)),
        Vertex(glm::vec3(halfSize, -halfSize, halfSize), glm::vec3(0, -1, 0), glm::vec2(1, 1)),
        Vertex(glm::vec3(-halfSize, -halfSize, halfSize), glm::vec3(0, -1, 0), glm::vec2(0, 1)),
        
        // 顶面 (Y+)
        Vertex(glm::vec3(-halfSize, halfSize, -halfSize), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
        Vertex(glm::vec3(halfSize, halfSize, -halfSize), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
        Vertex(glm::vec3(halfSize, halfSize, halfSize), glm::vec3(0, 1, 0), glm::vec2(1, 1)),
        Vertex(glm::vec3(-halfSize, halfSize, halfSize), glm::vec3(0, 1, 0), glm::vec2(0, 1))
    };
    
    // 立方体的12个三角形（6个面，每个面2个三角形）
    indices = {
        // 前面
        0, 1, 2, 2, 3, 0,
        // 后面
        4, 6, 5, 6, 4, 7,
        // 左面
        8, 9, 10, 10, 11, 8,
        // 右面
        12, 14, 13, 14, 12, 15,
        // 底面
        16, 18, 17, 18, 16, 19,
        // 顶面
        20, 21, 22, 22, 23, 20
    };
    
    auto mesh = std::make_shared<CMesh>(vertices, indices);
    mesh->calculateBoundingBox();
    return mesh;
}

std::shared_ptr<CMesh> MeshUtils::createSphere(float radius, unsigned int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // 生成球体顶点
    for (unsigned int y = 0; y <= segments; ++y) {
        for (unsigned int x = 0; x <= segments; ++x) {
            float xSegment = (float)x / (float)segments;
            float ySegment = (float)y / (float)segments;
            float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
            float yPos = std::cos(ySegment * M_PI);
            float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
            
            Vertex vertex;
            vertex.position = glm::vec3(xPos, yPos, zPos) * radius;
            vertex.normal = glm::vec3(xPos, yPos, zPos);
            vertex.texCoords = glm::vec2(xSegment, ySegment);
            vertices.push_back(vertex);
        }
    }
    
    // 生成索引
    for (unsigned int y = 0; y < segments; ++y) {
        for (unsigned int x = 0; x < segments; ++x) {
            indices.push_back((y + 1) * (segments + 1) + x);
            indices.push_back(y * (segments + 1) + x);
            indices.push_back(y * (segments + 1) + x + 1);
            
            indices.push_back((y + 1) * (segments + 1) + x + 1);
            indices.push_back(y * (segments + 1) + x + 1);
            indices.push_back((y + 1) * (segments + 1) + x);
        }
    }
    
    auto mesh = std::make_shared<CMesh>(vertices, indices);
    mesh->calculateBoundingBox();
    return mesh;
}

std::shared_ptr<CMesh> MeshUtils::createPlane(float width, float height, unsigned int widthSegments, unsigned int heightSegments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    // 生成平面顶点
    for (unsigned int y = 0; y <= heightSegments; ++y) {
        for (unsigned int x = 0; x <= widthSegments; ++x) {
            float xPos = (float)x / (float)widthSegments * width - halfWidth;
            float yPos = (float)y / (float)heightSegments * height - halfHeight;
            
            Vertex vertex;
            vertex.position = glm::vec3(xPos, 0.0f, yPos);
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            vertex.texCoords = glm::vec2((float)x / widthSegments, (float)y / heightSegments);
            vertices.push_back(vertex);
        }
    }
    
    // 生成索引
    for (unsigned int y = 0; y < heightSegments; ++y) {
        for (unsigned int x = 0; x < widthSegments; ++x) {
            indices.push_back(y * (widthSegments + 1) + x);
            indices.push_back((y + 1) * (widthSegments + 1) + x);
            indices.push_back(y * (widthSegments + 1) + x + 1);
            
            indices.push_back(y * (widthSegments + 1) + x + 1);
            indices.push_back((y + 1) * (widthSegments + 1) + x);
            indices.push_back((y + 1) * (widthSegments + 1) + x + 1);
        }
    }
    
    auto mesh = std::make_shared<CMesh>(vertices, indices);
    mesh->calculateBoundingBox();
    return mesh;
}

// 顶点计算
void MeshUtils::calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    std::vector<glm::vec3> normals(vertices.size(), glm::vec3(0.0f));
    
    // 计算面法线并累加到顶点
    for (size_t i = 0; i < indices.size(); i += 3) {
        if (i + 2 >= indices.size()) break;
        
        const Vertex& v0 = vertices[indices[i]];
        const Vertex& v1 = vertices[indices[i + 1]];
        const Vertex& v2 = vertices[indices[i + 2]];
        
        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        
        normals[indices[i]] += normal;
        normals[indices[i + 1]] += normal;
        normals[indices[i + 2]] += normal;
    }
    
    // 归一化顶点法线
    for (size_t i = 0; i < normals.size(); ++i) {
        vertices[i].normal = glm::normalize(normals[i]);
    }
}

void MeshUtils::calculateBoundingBox(const std::vector<Vertex>& vertices, CMesh::BoundingBox& bbox) {
    if (vertices.empty()) {
        bbox = CMesh::BoundingBox();
        return;
    }
    
    glm::vec3 minPos = vertices[0].position;
    glm::vec3 maxPos = vertices[0].position;
    
    for (const auto& vertex : vertices) {
        minPos = glm::min(minPos, vertex.position);
        maxPos = glm::max(maxPos, vertex.position);
    }
    
    bbox = CMesh::BoundingBox(minPos, maxPos);
}

// 包围盒可视化
std::shared_ptr<CMesh> MeshUtils::createBoundingBoxVisualization(const CMesh::BoundingBox& bbox) {
    if (!bbox.isValid) return nullptr;
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    glm::vec3 min = bbox.min;
    glm::vec3 max = bbox.max;
    
    // 8个角点
    vertices = {
        Vertex(glm::vec3(min.x, min.y, min.z)), // 0
        Vertex(glm::vec3(max.x, min.y, min.z)), // 1
        Vertex(glm::vec3(max.x, max.y, min.z)), // 2
        Vertex(glm::vec3(min.x, max.y, min.z)), // 3
        Vertex(glm::vec3(min.x, min.y, max.z)), // 4
        Vertex(glm::vec3(max.x, min.y, max.z)), // 5
        Vertex(glm::vec3(max.x, max.y, max.z)), // 6
        Vertex(glm::vec3(min.x, max.y, max.z))  // 7
    };
    
    // 12条边（24个顶点用于线框）
    indices = {
        0, 1, 1, 2, 2, 3, 3, 0,  // 前面
        4, 5, 5, 6, 6, 7, 7, 4,  // 后面
        0, 4, 1, 5, 2, 6, 3, 7   // 连接线
    };
    
    auto mesh = std::make_shared<CMesh>(vertices, indices, PrimitiveType::Lines);
    return mesh;
}

// 网格合并
std::shared_ptr<CMesh> MeshUtils::mergeMeshes(const std::vector<std::shared_ptr<CMesh>>& meshes) {
    std::vector<Vertex> allVertices;
    std::vector<unsigned int> allIndices;
    unsigned int vertexOffset = 0;
    
    for (const auto& mesh : meshes) {
        // 添加顶点
        const auto& vertices = mesh->getVertices();
        allVertices.insert(allVertices.end(), vertices.begin(), vertices.end());
        
        // 添加索引（调整偏移）
        const auto& indices = mesh->getIndices();
        for (unsigned int index : indices) {
            allIndices.push_back(index + vertexOffset);
        }
        
        vertexOffset += vertices.size();
    }
    
    auto mergedMesh = std::make_shared<CMesh>(allVertices, allIndices);
    mergedMesh->calculateBoundingBox();
    return mergedMesh;
}