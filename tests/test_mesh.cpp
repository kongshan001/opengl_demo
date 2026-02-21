/**
 * @file test_mesh.cpp
 * @brief Unit tests for CMesh class (non-OpenGL parts)
 * 
 * Tests BoundingBox and VertexAttributeLayout which don't require OpenGL context.
 */

#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "mesh/Vertex.h"
#include "mesh/Mesh.h"

// ============================================================================
// BoundingBox 测试（不需要 OpenGL 上下文）
// ============================================================================

class BoundingBoxTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

// 测试默认包围盒
TEST_F(BoundingBoxTest, DefaultConstructor) {
    CMesh::BoundingBox box;
    EXPECT_FALSE(box.isValid);
    EXPECT_FLOAT_EQ(box.min.x, 0.0f);
    EXPECT_FLOAT_EQ(box.max.x, 0.0f);
}

// 测试自定义包围盒
TEST_F(BoundingBoxTest, CustomConstructor) {
    glm::vec3 min(-1.0f, -2.0f, -3.0f);
    glm::vec3 max(1.0f, 2.0f, 3.0f);
    
    CMesh::BoundingBox box(min, max);
    EXPECT_TRUE(box.isValid);
    EXPECT_FLOAT_EQ(box.min.x, -1.0f);
    EXPECT_FLOAT_EQ(box.max.x, 1.0f);
}

// 测试包围盒中心点
TEST_F(BoundingBoxTest, GetCenter) {
    glm::vec3 min(-1.0f, -1.0f, -1.0f);
    glm::vec3 max(1.0f, 1.0f, 1.0f);
    
    CMesh::BoundingBox box(min, max);
    glm::vec3 center = box.getCenter();
    
    EXPECT_FLOAT_EQ(center.x, 0.0f);
    EXPECT_FLOAT_EQ(center.y, 0.0f);
    EXPECT_FLOAT_EQ(center.z, 0.0f);
}

// 测试包围盒尺寸
TEST_F(BoundingBoxTest, GetSize) {
    glm::vec3 min(0.0f, 0.0f, 0.0f);
    glm::vec3 max(2.0f, 3.0f, 4.0f);
    
    CMesh::BoundingBox box(min, max);
    glm::vec3 size = box.getSize();
    
    EXPECT_FLOAT_EQ(size.x, 2.0f);
    EXPECT_FLOAT_EQ(size.y, 3.0f);
    EXPECT_FLOAT_EQ(size.z, 4.0f);
}

// 测试包围盒半径
TEST_F(BoundingBoxTest, GetRadius) {
    glm::vec3 min(-2.0f, -4.0f, -6.0f);
    glm::vec3 max(2.0f, 4.0f, 6.0f);
    
    CMesh::BoundingBox box(min, max);
    glm::vec3 radius = box.getRadius();
    
    EXPECT_FLOAT_EQ(radius.x, 2.0f);
    EXPECT_FLOAT_EQ(radius.y, 4.0f);
    EXPECT_FLOAT_EQ(radius.z, 6.0f);
}

// 测试非对称包围盒
TEST_F(BoundingBoxTest, AsymmetricBox) {
    glm::vec3 min(1.0f, 2.0f, 3.0f);
    glm::vec3 max(4.0f, 6.0f, 9.0f);
    
    CMesh::BoundingBox box(min, max);
    glm::vec3 center = box.getCenter();
    
    EXPECT_FLOAT_EQ(center.x, 2.5f);
    EXPECT_FLOAT_EQ(center.y, 4.0f);
    EXPECT_FLOAT_EQ(center.z, 6.0f);
}

// ============================================================================
// VertexAttributeLayout 测试（不需要 OpenGL 上下文）
// ============================================================================

class VertexLayoutTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

// 测试 PositionOnly 布局
TEST_F(VertexLayoutTest, PositionOnly) {
    VertexAttributeLayout layout = VertexAttributeLayout::PositionOnly();
    
    EXPECT_EQ(layout.attributes.size(), 1u);
    EXPECT_EQ(layout.stride, 3 * sizeof(float));  // 3 floats for position
    EXPECT_EQ(layout.attributes[0].type, VertexAttribute::Position);
    EXPECT_EQ(layout.attributes[0].count, 3u);
}

// 测试 PositionNormal 布局
TEST_F(VertexLayoutTest, PositionNormal) {
    VertexAttributeLayout layout = VertexAttributeLayout::PositionNormal();
    
    EXPECT_EQ(layout.attributes.size(), 2u);
    EXPECT_EQ(layout.stride, 6 * sizeof(float));  // 3 + 3 floats
    EXPECT_EQ(layout.attributes[0].type, VertexAttribute::Position);
    EXPECT_EQ(layout.attributes[1].type, VertexAttribute::Normal);
}

// 测试 PositionNormalTex 布局
TEST_F(VertexLayoutTest, PositionNormalTex) {
    VertexAttributeLayout layout = VertexAttributeLayout::PositionNormalTex();
    
    EXPECT_EQ(layout.attributes.size(), 3u);
    EXPECT_EQ(layout.stride, 8 * sizeof(float));  // 3 + 3 + 2 floats
    EXPECT_EQ(layout.attributes[0].type, VertexAttribute::Position);
    EXPECT_EQ(layout.attributes[1].type, VertexAttribute::Normal);
    EXPECT_EQ(layout.attributes[2].type, VertexAttribute::TexCoords);
}

// 测试 Full 布局
TEST_F(VertexLayoutTest, Full) {
    VertexAttributeLayout layout = VertexAttributeLayout::Full();
    
    EXPECT_EQ(layout.attributes.size(), 5u);
    // 3 + 3 + 2 + 3 + 3 = 14 floats
    EXPECT_EQ(layout.stride, 14 * sizeof(float));
}

// 测试自定义布局
TEST_F(VertexLayoutTest, CustomLayout) {
    VertexAttributeLayout layout;
    layout.addAttribute(VertexAttribute::Position, 3);
    layout.addAttribute(VertexAttribute::TexCoords, 2);
    
    EXPECT_EQ(layout.attributes.size(), 2u);
    EXPECT_EQ(layout.stride, 5 * sizeof(float));
}

// 测试布局属性偏移
TEST_F(VertexLayoutTest, AttributeOffsets) {
    VertexAttributeLayout layout = VertexAttributeLayout::PositionNormalTex();
    
    // Position 在偏移 0
    EXPECT_EQ(layout.attributes[0].offset, 0u);
    // Normal 在偏移 12 (3 floats * 4 bytes)
    EXPECT_EQ(layout.attributes[1].offset, 12u);
    // TexCoords 在偏移 24 (6 floats * 4 bytes)
    EXPECT_EQ(layout.attributes[2].offset, 24u);
}

// 测试空布局
TEST_F(VertexLayoutTest, EmptyLayout) {
    VertexAttributeLayout layout;
    EXPECT_EQ(layout.attributes.size(), 0u);
    EXPECT_EQ(layout.stride, 0u);
}

// ============================================================================
// VertexAttribute 枚举测试
// ============================================================================

class VertexAttributeEnumTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(VertexAttributeEnumTest, EnumValues) {
    // 确保枚举值是按顺序的
    EXPECT_EQ(static_cast<int>(VertexAttribute::Position), 0);
    EXPECT_EQ(static_cast<int>(VertexAttribute::Normal), 1);
    EXPECT_EQ(static_cast<int>(VertexAttribute::TexCoords), 2);
    EXPECT_EQ(static_cast<int>(VertexAttribute::Tangent), 3);
    EXPECT_EQ(static_cast<int>(VertexAttribute::Bitangent), 4);
    EXPECT_EQ(static_cast<int>(VertexAttribute::Count), 5);
}
