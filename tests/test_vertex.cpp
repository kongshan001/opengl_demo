#include <gtest/gtest.h>
#include "mesh/Vertex.h"
#include <glm/glm.hpp>

class VertexTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(VertexTest, DefaultConstructor) {
    Vertex v;
    EXPECT_FLOAT_EQ(v.position.x, 0.0f);
    EXPECT_FLOAT_EQ(v.position.y, 0.0f);
    EXPECT_FLOAT_EQ(v.position.z, 0.0f);
    EXPECT_FLOAT_EQ(v.normal.x, 0.0f);
    EXPECT_FLOAT_EQ(v.normal.y, 0.0f);
    EXPECT_FLOAT_EQ(v.normal.z, 0.0f);
    EXPECT_FLOAT_EQ(v.texCoords.x, 0.0f);
    EXPECT_FLOAT_EQ(v.texCoords.y, 0.0f);
}

TEST_F(VertexTest, PositionConstructor) {
    glm::vec3 pos(1.0f, 2.0f, 3.0f);
    Vertex v(pos);
    
    EXPECT_FLOAT_EQ(v.position.x, 1.0f);
    EXPECT_FLOAT_EQ(v.position.y, 2.0f);
    EXPECT_FLOAT_EQ(v.position.z, 3.0f);
}

TEST_F(VertexTest, PositionNormalTexConstructor) {
    glm::vec3 pos(1.0f, 2.0f, 3.0f);
    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    glm::vec2 tex(0.5f, 0.5f);
    Vertex v(pos, normal, tex);
    
    EXPECT_FLOAT_EQ(v.position.x, 1.0f);
    EXPECT_FLOAT_EQ(v.position.y, 2.0f);
    EXPECT_FLOAT_EQ(v.position.z, 3.0f);
    EXPECT_FLOAT_EQ(v.normal.x, 0.0f);
    EXPECT_FLOAT_EQ(v.normal.y, 1.0f);
    EXPECT_FLOAT_EQ(v.normal.z, 0.0f);
    EXPECT_FLOAT_EQ(v.texCoords.x, 0.5f);
    EXPECT_FLOAT_EQ(v.texCoords.y, 0.5f);
}

TEST_F(VertexTest, FullConstructor) {
    glm::vec3 pos(1.0f, 2.0f, 3.0f);
    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    glm::vec2 tex(0.5f, 0.5f);
    glm::vec3 tangent(1.0f, 0.0f, 0.0f);
    glm::vec3 bitangent(0.0f, 0.0f, 1.0f);
    Vertex v(pos, normal, tex, tangent, bitangent);
    
    EXPECT_FLOAT_EQ(v.position.x, 1.0f);
    EXPECT_FLOAT_EQ(v.normal.y, 1.0f);
    EXPECT_FLOAT_EQ(v.texCoords.x, 0.5f);
    EXPECT_FLOAT_EQ(v.tangent.x, 1.0f);
    EXPECT_FLOAT_EQ(v.bitangent.z, 1.0f);
}

TEST_F(VertexTest, CopyConstructor) {
    Vertex v1(glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 0.5f));
    Vertex v2(v1);
    
    EXPECT_FLOAT_EQ(v2.position.x, 1.0f);
    EXPECT_FLOAT_EQ(v2.position.y, 2.0f);
    EXPECT_FLOAT_EQ(v2.position.z, 3.0f);
    EXPECT_FLOAT_EQ(v2.normal.y, 1.0f);
    EXPECT_FLOAT_EQ(v2.texCoords.x, 0.5f);
}

TEST_F(VertexTest, AssignmentOperator) {
    Vertex v1(glm::vec3(1.0f, 2.0f, 3.0f));
    Vertex v2;
    v2 = v1;
    
    EXPECT_FLOAT_EQ(v2.position.x, 1.0f);
    EXPECT_FLOAT_EQ(v2.position.y, 2.0f);
    EXPECT_FLOAT_EQ(v2.position.z, 3.0f);
}

TEST_F(VertexTest, VertexAttributeLayoutDefault) {
    VertexAttributeLayout layout = VertexAttributeLayout::PositionNormalTex();
    
    EXPECT_EQ(layout.attributes.size(), 3);
}

TEST_F(VertexTest, VertexAttributeLayoutPositionOnly) {
    VertexAttributeLayout layout = VertexAttributeLayout::PositionOnly();
    
    EXPECT_EQ(layout.attributes.size(), 1);
    EXPECT_EQ(layout.attributes[0].type, VertexAttribute::Position);
    EXPECT_EQ(layout.attributes[0].count, 3);
}

TEST_F(VertexTest, VertexAttributeLayoutFull) {
    VertexAttributeLayout layout = VertexAttributeLayout::Full();
    
    EXPECT_EQ(layout.attributes.size(), 5);
    EXPECT_EQ(layout.attributes[0].type, VertexAttribute::Position);
    EXPECT_EQ(layout.attributes[1].type, VertexAttribute::Normal);
    EXPECT_EQ(layout.attributes[2].type, VertexAttribute::TexCoords);
    EXPECT_EQ(layout.attributes[3].type, VertexAttribute::Tangent);
    EXPECT_EQ(layout.attributes[4].type, VertexAttribute::Bitangent);
}
