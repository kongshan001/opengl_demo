/**
 * @file test_mesh_extended.cpp
 * @brief Extended unit tests for CMesh class
 */

#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "mesh/Vertex.h"
#include "mesh/Mesh.h"

// ============================================================================
// CMesh 构造函数测试
// 注意：这些测试需要 OpenGL 上下文，在没有 GL 的环境中会崩溃
// ============================================================================

class MeshConstructorTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

// 需要 OpenGL 上下文
TEST_F(MeshConstructorTest, DISABLED_VertexOnlyConstructor) {
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, 1.0f, 0.0f))
    };
    
    CMesh mesh(vertices);
    EXPECT_EQ(mesh.getVertexCount(), 3u);
    EXPECT_EQ(mesh.getIndexCount(), 0u);
    EXPECT_FALSE(mesh.hasIndices());
}

// 需要 OpenGL 上下文
TEST_F(MeshConstructorTest, DISABLED_VertexAndIndexConstructor) {
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, 1.0f, 0.0f))
    };
    std::vector<unsigned int> indices = {0, 1, 2};
    
    CMesh mesh(vertices, indices);
    EXPECT_EQ(mesh.getVertexCount(), 3u);
    EXPECT_EQ(mesh.getIndexCount(), 3u);
    EXPECT_TRUE(mesh.hasIndices());
}

// 需要 OpenGL 上下文
TEST_F(MeshConstructorTest, DISABLED_CopyConstructor) {
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, 0.0f, 0.0f))
    };
    CMesh mesh1(vertices);
    CMesh mesh2(mesh1);
    
    EXPECT_EQ(mesh2.getVertexCount(), 2u);
}

// 需要 OpenGL 上下文
TEST_F(MeshConstructorTest, DISABLED_AssignmentOperator) {
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, 0.0f, 0.0f))
    };
    CMesh mesh1(vertices);
    CMesh mesh2;
    mesh2 = mesh1;
    
    EXPECT_EQ(mesh2.getVertexCount(), 2u);
}

// ============================================================================
// 顶点数据操作测试（需要 GL 上下文）
// ============================================================================

class MeshVertexDataTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

// 需要 OpenGL 上下文
TEST_F(MeshVertexDataTest, DISABLED_SetVertices) {
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, 1.0f, 0.0f))
    };
    
    CMesh* mesh = new CMesh();
    mesh->setVertices(vertices);
    EXPECT_EQ(mesh->getVertexCount(), 3u);
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshVertexDataTest, DISABLED_SetIndices) {
    CMesh* mesh = new CMesh();
    std::vector<unsigned int> indices = {0, 1, 2, 3, 4, 5};
    mesh->setIndices(indices);
    
    EXPECT_EQ(mesh->getIndexCount(), 6u);
    EXPECT_TRUE(mesh->hasIndices());
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshVertexDataTest, DISABLED_ClearVertices) {
    CMesh* mesh = new CMesh();
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f))
    };
    mesh->setVertices(vertices);
    EXPECT_EQ(mesh->getVertexCount(), 1u);
    
    mesh->setVertices(std::vector<Vertex>());
    EXPECT_EQ(mesh->getVertexCount(), 0u);
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshVertexDataTest, DISABLED_ClearIndices) {
    CMesh* mesh = new CMesh();
    std::vector<unsigned int> indices = {0, 1, 2};
    mesh->setIndices(indices);
    EXPECT_EQ(mesh->getIndexCount(), 3u);
    
    mesh->setIndices(std::vector<unsigned int>());
    EXPECT_EQ(mesh->getIndexCount(), 0u);
    EXPECT_FALSE(mesh->hasIndices());
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshVertexDataTest, DISABLED_AddVertex) {
    CMesh* mesh = new CMesh();
    std::vector<Vertex> vertices = {Vertex(glm::vec3(1.0f, 2.0f, 3.0f))};
    mesh->setVertices(vertices);
    
    EXPECT_EQ(mesh->getVertexCount(), 1u);
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshVertexDataTest, DISABLED_AddMultipleVertices) {
    CMesh* mesh = new CMesh();
    std::vector<Vertex> vertices;
    for (int i = 0; i < 10; i++) {
        vertices.push_back(Vertex(glm::vec3(i, i, i)));
    }
    mesh->setVertices(vertices);
    
    EXPECT_EQ(mesh->getVertexCount(), 10u);
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshVertexDataTest, DISABLED_AddIndex) {
    CMesh* mesh = new CMesh();
    std::vector<unsigned int> indices = {0, 1, 2};
    mesh->setIndices(indices);
    
    EXPECT_EQ(mesh->getIndexCount(), 3u);
    delete mesh;
}

// ============================================================================
// 材质关联测试（需要 GL 上下文）
// ============================================================================

class MeshMaterialTest : public ::testing::Test {
protected:
    void SetUp() override {
        material = std::make_shared<CMaterial>("TestMaterial");
    }
    
    void TearDown() override {
    }
    
    std::shared_ptr<CMaterial> material;
};

// 需要 OpenGL 上下文
TEST_F(MeshMaterialTest, DISABLED_SetMaterial) {
    CMesh* mesh = new CMesh();
    mesh->setMaterial(material);
    EXPECT_TRUE(mesh->hasMaterial());
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshMaterialTest, DISABLED_HasMaterialFalse) {
    CMesh* mesh = new CMesh();
    EXPECT_FALSE(mesh->hasMaterial());
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshMaterialTest, DISABLED_GetMaterial) {
    CMesh* mesh = new CMesh();
    mesh->setMaterial(material);
    std::shared_ptr<CMaterial> mat = mesh->getMaterial();
    EXPECT_NE(mat, nullptr);
    EXPECT_EQ(mat->getName(), "TestMaterial");
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshMaterialTest, DISABLED_ClearMaterial) {
    CMesh* mesh = new CMesh();
    mesh->setMaterial(material);
    mesh->setMaterial(nullptr);
    EXPECT_FALSE(mesh->hasMaterial());
    delete mesh;
}

// ============================================================================
// 图元类型测试（需要 GL 上下文）
// ============================================================================

class MeshPrimitiveTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

// 需要 OpenGL 上下文
TEST_F(MeshPrimitiveTest, DISABLED_DefaultPrimitiveType) {
    CMesh* mesh = new CMesh();
    EXPECT_EQ(mesh->getPrimitiveType(), PrimitiveType::Triangles);
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshPrimitiveTest, DISABLED_SetPrimitiveType) {
    CMesh* mesh = new CMesh();
    mesh->setPrimitiveType(PrimitiveType::Lines);
    EXPECT_EQ(mesh->getPrimitiveType(), PrimitiveType::Lines);
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshPrimitiveTest, DISABLED_SetPoints) {
    CMesh* mesh = new CMesh();
    mesh->setPrimitiveType(PrimitiveType::Points);
    EXPECT_EQ(mesh->getPrimitiveType(), PrimitiveType::Points);
    delete mesh;
}

// 需要 OpenGL 上下文
TEST_F(MeshPrimitiveTest, DISABLED_SetTriangleStrip) {
    CMesh* mesh = new CMesh();
    mesh->setPrimitiveType(PrimitiveType::TriangleStrip);
    EXPECT_EQ(mesh->getPrimitiveType(), PrimitiveType::TriangleStrip);
    delete mesh;
}

// ============================================================================
// 边界测试（需要 GL 上下文）
// ============================================================================

class MeshBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

// 需要 OpenGL 上下文
TEST_F(MeshBoundaryTest, DISABLED_EmptyVertices) {
    std::vector<Vertex> empty;
    EXPECT_NO_THROW(CMesh mesh(empty));
}

// 需要 OpenGL 上下文
TEST_F(MeshBoundaryTest, DISABLED_EmptyIndices) {
    std::vector<Vertex> vertices = {Vertex(glm::vec3(0, 0, 0))};
    std::vector<unsigned int> empty;
    EXPECT_NO_THROW(CMesh mesh(vertices, empty));
}

// 需要 OpenGL 上下文
TEST_F(MeshBoundaryTest, DISABLED_LargeVertexCount) {
    std::vector<Vertex> vertices(100000);
    EXPECT_NO_THROW(CMesh mesh(vertices));
}

// 需要 OpenGL 上下文
TEST_F(MeshBoundaryTest, DISABLED_SingleVertex) {
    std::vector<Vertex> vertices = {Vertex(glm::vec3(1, 2, 3))};
    CMesh mesh(vertices);
    EXPECT_EQ(mesh.getVertexCount(), 1u);
}

// 需要 OpenGL 上下文
TEST_F(MeshBoundaryTest, DISABLED_DuplicateVertices) {
    std::vector<Vertex> vertices(3, Vertex(glm::vec3(1, 1, 1)));
    EXPECT_NO_THROW(CMesh mesh(vertices));
}

// ============================================================================
// 包围盒测试（需要 GL 上下文）
// ============================================================================

class MeshBoundingBoxExtendedTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

// 需要 OpenGL 上下文
TEST_F(MeshBoundingBoxExtendedTest, DISABLED_UnitCube) {
    // 单位立方体顶点
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f)),
        Vertex(glm::vec3( 0.5f, -0.5f, -0.5f)),
        Vertex(glm::vec3( 0.5f,  0.5f, -0.5f)),
        Vertex(glm::vec3(-0.5f,  0.5f, -0.5f)),
        Vertex(glm::vec3(-0.5f, -0.5f,  0.5f)),
        Vertex(glm::vec3( 0.5f, -0.5f,  0.5f)),
        Vertex(glm::vec3( 0.5f,  0.5f,  0.5f)),
        Vertex(glm::vec3(-0.5f,  0.5f,  0.5f))
    };
    
    CMesh mesh(vertices);
    CMesh::BoundingBox box = mesh.getBoundingBox();
    
    EXPECT_TRUE(box.isValid);
    EXPECT_FLOAT_EQ(box.getSize().x, 1.0f);
    EXPECT_FLOAT_EQ(box.getSize().y, 1.0f);
    EXPECT_FLOAT_EQ(box.getSize().z, 1.0f);
}

// 需要 OpenGL 上下文
TEST_F(MeshBoundingBoxExtendedTest, DISABLED_OffCenterMesh) {
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(10.0f, 10.0f, 10.0f)),
        Vertex(glm::vec3(12.0f, 10.0f, 10.0f)),
        Vertex(glm::vec3(10.0f, 15.0f, 10.0f))
    };
    
    CMesh mesh(vertices);
    CMesh::BoundingBox box = mesh.getBoundingBox();
    
    EXPECT_TRUE(box.isValid);
    EXPECT_FLOAT_EQ(box.getSize().x, 2.0f);
    EXPECT_FLOAT_EQ(box.getSize().y, 5.0f);
    EXPECT_FLOAT_EQ(box.getSize().z, 0.0f);  // Z 方向没有变化
}

// main 函数由测试框架提供
// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
