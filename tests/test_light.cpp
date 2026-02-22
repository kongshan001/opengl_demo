/**
 * @file test_light.cpp
 * @brief Light 类单元测试
 */

#include <gtest/gtest.h>
#include "lighting/Light.h"
#include <cmath>

// ============== DirectionalLight 测试 ==============

TEST(DirectionalLightTest, DefaultConstruction) {
    DirectionalLight light("test_dir");
    
    EXPECT_EQ(light.getType(), LightType::Directional);
    EXPECT_EQ(light.getName(), "test_dir");
    EXPECT_TRUE(light.isEnabled());
    EXPECT_FLOAT_EQ(light.getIntensity(), 1.0f);
    EXPECT_EQ(light.getColor(), glm::vec3(1.0f));
    EXPECT_EQ(light.getDirection(), glm::vec3(0.0f, -1.0f, 0.0f));
}

TEST(DirectionalLightTest, SetDirection) {
    DirectionalLight light;
    light.setDirection(glm::vec3(1.0f, 2.0f, 3.0f));
    
    glm::vec3 dir = light.getDirection();
    glm::vec3 expected = glm::normalize(glm::vec3(1.0f, 2.0f, 3.0f));
    
    EXPECT_NEAR(dir.x, expected.x, 0.0001f);
    EXPECT_NEAR(dir.y, expected.y, 0.0001f);
    EXPECT_NEAR(dir.z, expected.z, 0.0001f);
}

TEST(DirectionalLightTest, SetColorAndIntensity) {
    DirectionalLight light;
    light.setColor(glm::vec3(1.0f, 0.5f, 0.0f));
    light.setIntensity(0.8f);
    
    EXPECT_EQ(light.getColor(), glm::vec3(1.0f, 0.5f, 0.0f));
    EXPECT_FLOAT_EQ(light.getIntensity(), 0.8f);
}

TEST(DirectionalLightTest, EnableDisable) {
    DirectionalLight light;
    EXPECT_TRUE(light.isEnabled());
    
    light.setEnabled(false);
    EXPECT_FALSE(light.isEnabled());
    
    light.setEnabled(true);
    EXPECT_TRUE(light.isEnabled());
}

TEST(DirectionalLightTest, ShaderData) {
    DirectionalLight light;
    light.setDirection(glm::vec3(0.0f, 1.0f, 0.0f));
    light.setColor(glm::vec3(1.0f, 0.0f, 0.0f));
    light.setIntensity(0.5f);
    
    glm::vec3 pos, dir, color;
    float intensity, constant, linear, quadratic, innerCutoff, outerCutoff;
    
    light.getShaderData(pos, dir, color, intensity, constant, linear, quadratic,
                        innerCutoff, outerCutoff);
    
    EXPECT_EQ(pos, glm::vec3(0.0f));
    EXPECT_EQ(dir, glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_EQ(color, glm::vec3(1.0f, 0.0f, 0.0f));
    EXPECT_FLOAT_EQ(intensity, 0.5f);
}

// ============== PointLight 测试 ==============

TEST(PointLightTest, DefaultConstruction) {
    PointLight light("test_point");
    
    EXPECT_EQ(light.getType(), LightType::Point);
    EXPECT_EQ(light.getName(), "test_point");
    EXPECT_TRUE(light.isEnabled());
    EXPECT_EQ(light.getPosition(), glm::vec3(0.0f));
    EXPECT_FLOAT_EQ(light.getConstant(), 1.0f);
    EXPECT_FLOAT_EQ(light.getLinear(), 0.09f);
    EXPECT_FLOAT_EQ(light.getQuadratic(), 0.032f);
}

TEST(PointLightTest, SetPosition) {
    PointLight light;
    light.setPosition(glm::vec3(5.0f, 3.0f, -2.0f));
    
    EXPECT_EQ(light.getPosition(), glm::vec3(5.0f, 3.0f, -2.0f));
}

TEST(PointLightTest, SetAttenuation) {
    PointLight light;
    light.setAttenuation(1.0f, 0.045f, 0.0075f);
    
    EXPECT_FLOAT_EQ(light.getConstant(), 1.0f);
    EXPECT_FLOAT_EQ(light.getLinear(), 0.045f);
    EXPECT_FLOAT_EQ(light.getQuadratic(), 0.0075f);
}

TEST(PointLightTest, ShaderData) {
    PointLight light;
    light.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    light.setColor(glm::vec3(0.0f, 1.0f, 0.0f));
    light.setIntensity(2.0f);
    light.setAttenuation(1.0f, 0.1f, 0.05f);
    
    glm::vec3 pos, dir, color;
    float intensity, constant, linear, quadratic, innerCutoff, outerCutoff;
    
    light.getShaderData(pos, dir, color, intensity, constant, linear, quadratic,
                        innerCutoff, outerCutoff);
    
    EXPECT_EQ(pos, glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(dir, glm::vec3(0.0f));
    EXPECT_EQ(color, glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_FLOAT_EQ(intensity, 2.0f);
    EXPECT_FLOAT_EQ(constant, 1.0f);
    EXPECT_FLOAT_EQ(linear, 0.1f);
    EXPECT_FLOAT_EQ(quadratic, 0.05f);
}

// ============== SpotLight 测试 ==============

TEST(SpotLightTest, DefaultConstruction) {
    SpotLight light("test_spot");
    
    EXPECT_EQ(light.getType(), LightType::Spotlight);
    EXPECT_EQ(light.getName(), "test_spot");
    EXPECT_TRUE(light.isEnabled());
    EXPECT_EQ(light.getPosition(), glm::vec3(0.0f));
    EXPECT_EQ(light.getDirection(), glm::vec3(0.0f, -1.0f, 0.0f));
    
    // 默认内锥角 12.5°，外锥角 17.5°
    float expectedInner = cosf(glm::radians(12.5f));
    float expectedOuter = cosf(glm::radians(17.5f));
    EXPECT_NEAR(light.getInnerCutoff(), expectedInner, 0.0001f);
    EXPECT_NEAR(light.getOuterCutoff(), expectedOuter, 0.0001f);
}

TEST(SpotLightTest, SetPositionAndDirection) {
    SpotLight light;
    light.setPosition(glm::vec3(0.0f, 10.0f, 0.0f));
    light.setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
    
    EXPECT_EQ(light.getPosition(), glm::vec3(0.0f, 10.0f, 0.0f));
    EXPECT_EQ(light.getDirection(), glm::vec3(0.0f, -1.0f, 0.0f));
}

TEST(SpotLightTest, SetCutoff) {
    SpotLight light;
    light.setCutoff(15.0f, 30.0f);
    
    float expectedInner = cosf(glm::radians(15.0f));
    float expectedOuter = cosf(glm::radians(30.0f));
    
    EXPECT_NEAR(light.getInnerCutoff(), expectedInner, 0.0001f);
    EXPECT_NEAR(light.getOuterCutoff(), expectedOuter, 0.0001f);
}

TEST(SpotLightTest, ShaderData) {
    SpotLight light;
    light.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    light.setDirection(glm::vec3(1.0f, 0.0f, 0.0f));
    light.setCutoff(10.0f, 20.0f);
    light.setAttenuation(1.0f, 0.05f, 0.01f);
    
    glm::vec3 pos, dir, color;
    float intensity, constant, linear, quadratic, innerCutoff, outerCutoff;
    
    light.getShaderData(pos, dir, color, intensity, constant, linear, quadratic,
                        innerCutoff, outerCutoff);
    
    EXPECT_EQ(pos, glm::vec3(0.0f, 5.0f, 0.0f));
    EXPECT_EQ(dir, glm::vec3(1.0f, 0.0f, 0.0f));
    EXPECT_FLOAT_EQ(constant, 1.0f);
    EXPECT_FLOAT_EQ(linear, 0.05f);
    EXPECT_FLOAT_EQ(quadratic, 0.01f);
}
