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

std::shared_ptr<CMesh> MeshUtils::createCylinder(float radius, float height, unsigned int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float halfHeight = height * 0.5f;
    float angleStep = 2.0f * M_PI / segments;
    
    // 侧面顶点
    for (unsigned int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = std::cos(angle) * radius;
        float z = std::sin(angle) * radius;
        float u = (float)i / segments;
        
        // 底部顶点
        Vertex bottom;
        bottom.position = glm::vec3(x, -halfHeight, z);
        bottom.normal = glm::vec3(std::cos(angle), 0.0f, std::sin(angle));
        bottom.texCoords = glm::vec2(u, 1.0f);
        vertices.push_back(bottom);
        
        // 顶部顶点
        Vertex top;
        top.position = glm::vec3(x, halfHeight, z);
        top.normal = glm::vec3(std::cos(angle), 0.0f, std::sin(angle));
        top.texCoords = glm::vec2(u, 0.0f);
        vertices.push_back(top);
    }
    
    // 侧面索引
    for (unsigned int i = 0; i < segments; ++i) {
        unsigned int bottomLeft = i * 2;
        unsigned int topLeft = i * 2 + 1;
        unsigned int bottomRight = (i + 1) * 2;
        unsigned int topRight = (i + 1) * 2 + 1;
        
        indices.push_back(bottomLeft);
        indices.push_back(topLeft);
        indices.push_back(bottomRight);
        
        indices.push_back(bottomRight);
        indices.push_back(topLeft);
        indices.push_back(topRight);
    }
    
    // 底面圆盘
    unsigned int bottomCenterIdx = vertices.size();
    Vertex bottomCenter;
    bottomCenter.position = glm::vec3(0.0f, -halfHeight, 0.0f);
    bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
    bottomCenter.texCoords = glm::vec2(0.5f, 0.5f);
    vertices.push_back(bottomCenter);
    
    for (unsigned int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        Vertex v;
        v.position = glm::vec3(std::cos(angle) * radius, -halfHeight, std::sin(angle) * radius);
        v.normal = glm::vec3(0.0f, -1.0f, 0.0f);
        v.texCoords = glm::vec2(std::cos(angle) * 0.5f + 0.5f, std::sin(angle) * 0.5f + 0.5f);
        vertices.push_back(v);
    }
    
    for (unsigned int i = 0; i < segments; ++i) {
        indices.push_back(bottomCenterIdx);
        indices.push_back(bottomCenterIdx + i + 2);
        indices.push_back(bottomCenterIdx + i + 1);
    }
    
    // 顶面圆盘
    unsigned int topCenterIdx = vertices.size();
    Vertex topCenter;
    topCenter.position = glm::vec3(0.0f, halfHeight, 0.0f);
    topCenter.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    topCenter.texCoords = glm::vec2(0.5f, 0.5f);
    vertices.push_back(topCenter);
    
    for (unsigned int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        Vertex v;
        v.position = glm::vec3(std::cos(angle) * radius, halfHeight, std::sin(angle) * radius);
        v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        v.texCoords = glm::vec2(std::cos(angle) * 0.5f + 0.5f, std::sin(angle) * 0.5f + 0.5f);
        vertices.push_back(v);
    }
    
    for (unsigned int i = 0; i < segments; ++i) {
        indices.push_back(topCenterIdx);
        indices.push_back(topCenterIdx + i + 1);
        indices.push_back(topCenterIdx + i + 2);
    }
    
    auto mesh = std::make_shared<CMesh>(vertices, indices);
    mesh->calculateBoundingBox();
    return mesh;
}

std::shared_ptr<CMesh> MeshUtils::createCone(float radius, float height, unsigned int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float halfHeight = height * 0.5f;
    float angleStep = 2.0f * M_PI / segments;
    
    // 侧面顶点（尖端在顶部）
    for (unsigned int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = std::cos(angle) * radius;
        float z = std::sin(angle) * radius;
        float u = (float)i / segments;
        
        // 底部顶点
        Vertex bottom;
        bottom.position = glm::vec3(x, -halfHeight, z);
        // 计算侧面法线
        glm::vec3 sideNormal = glm::normalize(glm::vec3(radius, height / 2.0f, 0.0f));
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        bottom.normal = glm::normalize(glm::vec3(sideNormal.x * cosA, sideNormal.y, sideNormal.x * sinA));
        bottom.texCoords = glm::vec2(u, 1.0f);
        vertices.push_back(bottom);
        
        // 顶部尖端（共享多个顶点）
        Vertex apex;
        apex.position = glm::vec3(0.0f, halfHeight, 0.0f);
        apex.normal = bottom.normal;  // 每个顶点有自己的法线朝向
        apex.texCoords = glm::vec2(u, 0.0f);
        vertices.push_back(apex);
    }
    
    // 侧面索引
    for (unsigned int i = 0; i < segments; ++i) {
        unsigned int bottomLeft = i * 2;
        unsigned int apexLeft = i * 2 + 1;
        unsigned int bottomRight = (i + 1) * 2;
        unsigned int apexRight = (i + 1) * 2 + 1;
        
        indices.push_back(bottomLeft);
        indices.push_back(apexLeft);
        indices.push_back(bottomRight);
    }
    
    // 底面圆盘
    unsigned int bottomCenterIdx = vertices.size();
    Vertex bottomCenter;
    bottomCenter.position = glm::vec3(0.0f, -halfHeight, 0.0f);
    bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
    bottomCenter.texCoords = glm::vec2(0.5f, 0.5f);
    vertices.push_back(bottomCenter);
    
    for (unsigned int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        Vertex v;
        v.position = glm::vec3(std::cos(angle) * radius, -halfHeight, std::sin(angle) * radius);
        v.normal = glm::vec3(0.0f, -1.0f, 0.0f);
        v.texCoords = glm::vec2(std::cos(angle) * 0.5f + 0.5f, std::sin(angle) * 0.5f + 0.5f);
        vertices.push_back(v);
    }
    
    for (unsigned int i = 0; i < segments; ++i) {
        indices.push_back(bottomCenterIdx);
        indices.push_back(bottomCenterIdx + i + 2);
        indices.push_back(bottomCenterIdx + i + 1);
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

// 圆环生成
std::shared_ptr<CMesh> MeshUtils::createTorus(float outerRadius, float innerRadius, unsigned int sides, unsigned int rings) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float tubeRadius = (outerRadius - innerRadius) * 0.5f;
    float ringRadius = innerRadius + tubeRadius;
    
    // 生成顶点
    for (unsigned int ring = 0; ring <= rings; ring++) {
        float theta = static_cast<float>(ring) / rings * 2.0f * M_PI;
        
        for (unsigned int side = 0; side <= sides; side++) {
            float phi = static_cast<float>(side) / sides * 2.0f * M_PI;
            
            // 计算位置
            float x = (ringRadius + tubeRadius * cos(phi)) * cos(theta);
            float y = tubeRadius * sin(phi);
            float z = (ringRadius + tubeRadius * cos(phi)) * sin(theta);
            
            // 计算法线
            float nx = cos(phi) * cos(theta);
            float ny = sin(phi);
            float nz = cos(phi) * sin(theta);
            
            // 计算纹理坐标
            float u = static_cast<float>(ring) / rings;
            float v = static_cast<float>(side) / sides;
            
            vertices.push_back(Vertex(
                glm::vec3(x, y, z),
                glm::vec3(nx, ny, nz),
                glm::vec2(u, v)
            ));
        }
    }
    
    // 生成索引
    for (unsigned int ring = 0; ring < rings; ring++) {
        for (unsigned int side = 0; side < sides; side++) {
            unsigned int current = ring * (sides + 1) + side;
            unsigned int next = current + sides + 1;
            
            // 两个三角形组成一个四边形
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);
            
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
    
    auto mesh = std::make_shared<CMesh>(vertices, indices);
    mesh->calculateBoundingBox();
    return mesh;
}

// 胶囊体生成
std::shared_ptr<CMesh> MeshUtils::createCapsule(float radius, float height, unsigned int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float halfHeight = height * 0.5f;
    unsigned int rings = segments / 2;  // 每个半球的环数
    
    // 生成上半球
    for (unsigned int ring = 0; ring <= rings; ring++) {
        float theta = static_cast<float>(ring) / rings * M_PI * 0.5f;  // 0 到 PI/2
        
        for (unsigned int seg = 0; seg <= segments; seg++) {
            float phi = static_cast<float>(seg) / segments * 2.0f * M_PI;
            
            float x = radius * sin(theta) * cos(phi);
            float y = halfHeight + radius * cos(theta);
            float z = radius * sin(theta) * sin(phi);
            
            float nx = sin(theta) * cos(phi);
            float ny = cos(theta);
            float nz = sin(theta) * sin(phi);
            
            float u = static_cast<float>(seg) / segments;
            float v = static_cast<float>(ring) / rings;
            
            vertices.push_back(Vertex(
                glm::vec3(x, y, z),
                glm::vec3(nx, ny, nz),
                glm::vec2(u, v)
            ));
        }
    }
    
    // 生成圆柱体中间部分
    unsigned int cylinderTopIndex = vertices.size();
    for (unsigned int ring = 0; ring <= 1; ring++) {
        float y = halfHeight - (static_cast<float>(ring) * height);
        
        for (unsigned int seg = 0; seg <= segments; seg++) {
            float phi = static_cast<float>(seg) / segments * 2.0f * M_PI;
            
            float x = radius * cos(phi);
            float z = radius * sin(phi);
            
            float u = static_cast<float>(seg) / segments;
            float v = 0.5f;
            
            vertices.push_back(Vertex(
                glm::vec3(x, y, z),
                glm::vec3(cos(phi), 0.0f, sin(phi)),
                glm::vec2(u, v)
            ));
        }
    }
    
    // 生成下半球
    unsigned int bottomHemisphereStart = vertices.size();
    for (unsigned int ring = 0; ring <= rings; ring++) {
        float theta = M_PI * 0.5f + static_cast<float>(ring) / rings * M_PI * 0.5f;  // PI/2 到 PI
        
        for (unsigned int seg = 0; seg <= segments; seg++) {
            float phi = static_cast<float>(seg) / segments * 2.0f * M_PI;
            
            float x = radius * sin(theta) * cos(phi);
            float y = -halfHeight + radius * cos(theta);
            float z = radius * sin(theta) * sin(phi);
            
            float nx = sin(theta) * cos(phi);
            float ny = cos(theta);
            float nz = sin(theta) * sin(phi);
            
            float u = static_cast<float>(seg) / segments;
            float v = static_cast<float>(ring) / rings;
            
            vertices.push_back(Vertex(
                glm::vec3(x, y, z),
                glm::vec3(nx, ny, nz),
                glm::vec2(u, v)
            ));
        }
    }
    
    // 生成上半球索引
    for (unsigned int ring = 0; ring < rings; ring++) {
        for (unsigned int seg = 0; seg < segments; seg++) {
            unsigned int current = ring * (segments + 1) + seg;
            unsigned int next = current + segments + 1;
            
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);
            
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
    
    // 生成圆柱体中间部分索引
    for (unsigned int seg = 0; seg < segments; seg++) {
        unsigned int current = cylinderTopIndex + seg;
        unsigned int next = current + segments + 1;
        
        indices.push_back(current);
        indices.push_back(next);
        indices.push_back(current + 1);
        
        indices.push_back(current + 1);
        indices.push_back(next);
        indices.push_back(next + 1);
    }
    
    // 生成下半球索引
    for (unsigned int ring = 0; ring < rings; ring++) {
        for (unsigned int seg = 0; seg < segments; seg++) {
            unsigned int current = bottomHemisphereStart + ring * (segments + 1) + seg;
            unsigned int next = current + segments + 1;
            
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);
            
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
    
    auto mesh = std::make_shared<CMesh>(vertices, indices);
    mesh->calculateBoundingBox();
    return mesh;
}