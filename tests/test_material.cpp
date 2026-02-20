#include <gtest/gtest.h>
#include "mesh/Material.h"
#include <glm/glm.hpp>

class MaterialTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(MaterialTest, DefaultConstructor) {
    CMaterial material;
    
    EXPECT_EQ(material.getName(), "DefaultMaterial");
    EXPECT_FLOAT_EQ(material.diffuseColor.r, 1.0f);
    EXPECT_FLOAT_EQ(material.specularColor.r, 0.5f);
    EXPECT_FLOAT_EQ(material.ambientColor.r, 0.1f);
    EXPECT_FLOAT_EQ(material.shininess, 32.0f);
    EXPECT_FLOAT_EQ(material.opacity, 1.0f);
    EXPECT_FALSE(material.hasShader());
    EXPECT_FALSE(material.hasTextures());
    EXPECT_FALSE(material.isTransparent());
}

TEST_F(MaterialTest, NamedConstructor) {
    CMaterial material("TestMaterial");
    EXPECT_EQ(material.getName(), "TestMaterial");
}

TEST_F(MaterialTest, SetName) {
    CMaterial material;
    material.setName("NewName");
    EXPECT_EQ(material.getName(), "NewName");
}

TEST_F(MaterialTest, SetColors) {
    CMaterial material;
    glm::vec3 diffuse(1.0f, 0.5f, 0.2f);
    glm::vec3 specular(1.0f, 1.0f, 1.0f);
    glm::vec3 ambient(0.1f, 0.1f, 0.1f);
    
    material.setColors(diffuse, specular, ambient);
    
    EXPECT_FLOAT_EQ(material.diffuseColor.r, 1.0f);
    EXPECT_FLOAT_EQ(material.diffuseColor.g, 0.5f);
    EXPECT_FLOAT_EQ(material.diffuseColor.b, 0.2f);
    EXPECT_FLOAT_EQ(material.specularColor.r, 1.0f);
    EXPECT_FLOAT_EQ(material.ambientColor.r, 0.1f);
}

TEST_F(MaterialTest, SetProperties) {
    CMaterial material;
    material.setProperties(64.0f, 0.8f, 0.5f);
    
    EXPECT_FLOAT_EQ(material.shininess, 64.0f);
    EXPECT_FLOAT_EQ(material.specularStrength, 0.8f);
    EXPECT_FLOAT_EQ(material.opacity, 0.5f);
    EXPECT_TRUE(material.isTransparent());
}

TEST_F(MaterialTest, CopyConstructor) {
    CMaterial material1("Original");
    material1.setColors(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.1f));
    
    CMaterial material2(material1);
    
    EXPECT_EQ(material2.getName(), "Original");
    EXPECT_FLOAT_EQ(material2.diffuseColor.r, 1.0f);
    EXPECT_FLOAT_EQ(material2.diffuseColor.g, 0.0f);
}

TEST_F(MaterialTest, AssignmentOperator) {
    CMaterial material1("Original");
    material1.setColors(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.1f));
    
    CMaterial material2;
    material2 = material1;
    
    EXPECT_EQ(material2.getName(), "Original");
    EXPECT_FLOAT_EQ(material2.diffuseColor.r, 1.0f);
}

TEST_F(MaterialTest, TextureCount) {
    CMaterial material;
    EXPECT_EQ(material.getTextureCount(), 0);
    EXPECT_FALSE(material.hasTextures());
}

TEST_F(MaterialTest, ClearTextures) {
    CMaterial material;
    material.clearTextures();
    EXPECT_EQ(material.getTextureCount(), 0);
}

TEST_F(MaterialTest, GetTextureInvalid) {
    CMaterial material;
    EXPECT_EQ(material.getTexture(0), nullptr);
    EXPECT_EQ(material.getTexture(100), nullptr);
}

TEST_F(MaterialTest, GetTextureByTypeNone) {
    CMaterial material;
    EXPECT_EQ(material.getTextureByType(TextureType::Diffuse), nullptr);
}
