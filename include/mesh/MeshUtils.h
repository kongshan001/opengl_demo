#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "mesh/Mesh.h"
#include "mesh/Vertex.h"

class MeshUtils {
public:
    // 基础几何体生成
    static std::shared_ptr<CMesh> createCube(float size = 1.0f);
    static std::shared_ptr<CMesh> createSphere(float radius = 1.0f, unsigned int segments = 32);
    static std::shared_ptr<CMesh> createPlane(float width = 1.0f, float height = 1.0f, unsigned int widthSegments = 1, unsigned int heightSegments = 1);
    static std::shared_ptr<CMesh> createCylinder(float radius = 1.0f, float height = 1.0f, unsigned int segments = 32);
    static std::shared_ptr<CMesh> createCone(float radius = 1.0f, float height = 1.0f, unsigned int segments = 32);
    
    // 顶点计算
    static void calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    static void calculateTangentsAndBitangents(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    static void calculateBoundingBox(const std::vector<Vertex>& vertices, CMesh::BoundingBox& bbox);
    
    // 顶点索引化（消除重复顶点）
    static std::vector<unsigned int> indexVertices(const std::vector<Vertex>& vertices, std::vector<Vertex>& indexedVertices);
    
    // 顶点数据合并
    static std::shared_ptr<CMesh> mergeMeshes(const std::vector<std::shared_ptr<CMesh>>& meshes);
    
    // 网格细分
    static std::shared_ptr<CMesh> subdivideMesh(std::shared_ptr<CMesh> mesh, unsigned int subdivisions = 1);
    
    // 法线可视化（生成法线线段网格）
    static std::shared_ptr<CMesh> createNormalVisualization(std::shared_ptr<CMesh> mesh, float normalLength = 0.1f);
    
    // 包围盒可视化
    static std::shared_ptr<CMesh> createBoundingBoxVisualization(const CMesh::BoundingBox& bbox);

private:
    // 辅助函数
    static void generateSphereVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                                    float radius, unsigned int segments);
    static void generatePlaneVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                                   float width, float height, unsigned int widthSegments, unsigned int heightSegments);
    
    // 数学辅助
    static glm::vec3 calculateTangent(const Vertex& v0, const Vertex& v1, const Vertex& v2);
    static float hashVertex(const Vertex& vertex);
    static bool areVerticesEqual(const Vertex& a, const Vertex& b);
};

#endif