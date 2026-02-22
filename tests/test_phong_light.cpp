/**
 * @file test_phong_light.cpp
 * @brief PhongLight DirectionalLightPhong PointLightPhong PhongLightManager unit tests
 */

#include <gtest/gtest.h>
#include "light/Light.h"
#include "light/LightManager.h"
#include <cmath>

// ============== DirectionalLightPhong Tests ==============

TEST(DirectionalLightPhongTest, DefaultConstruction) {
    DirectionalLightPhong light("test_dir");
    
    EXPECT_EQ(light.getType(), LightTypePhong::Directional);
    EXPECT_EQ(light.getName(), "test_dir");
    EXPECT_TRUE(light.isEnabled());
    EXPECT_FLOAT_EQ(light.getIntensity(), 1.0f);
    EXPECT_EQ(light.getColor(), glm::vec3(1.0f));
    EXPECT_EQ(light.getDirection(), glm::vec3(0.0f, -1.0f, 0.0f));
}

TEST(DirectionalLightPhongTest, DefaultConstructionEmptyName) {
    DirectionalLightPhong light;
    EXPECT_EQ(light.getName(), "");
}

TEST(DirectionalLightPhongTest, SetDirection) {
    DirectionalLightPhong light;
    light.setDirection(glm::vec3(1.0f, 2.0f, 3.0f));
    
    glm::vec3 dir = light.getDirection();
    glm::vec3 expected = glm::normalize(glm::vec3(1.0f, 2.0f, 3.0f));
    
    EXPECT_NEAR(dir.x, expected.x, 0.0001f);
    EXPECT_NEAR(dir.y, expected.y, 0.0001f);
    EXPECT_NEAR(dir.z, expected.z, 0.0001f);
}

TEST(DirectionalLightPhongTest, SetDirectionNormalized) {
    DirectionalLightPhong light;
    light.setDirection(glm::vec3(0.0f, -5.0f, 0.0f));
    
    glm::vec3 dir = light.getDirection();
    EXPECT_NEAR(dir.x, 0.0f, 0.0001f);
    EXPECT_NEAR(dir.y, -1.0f, 0.0001f);
    EXPECT_NEAR(dir.z, 0.0f, 0.0001f);
}

TEST(DirectionalLightPhongTest, SetColorAndIntensity) {
    DirectionalLightPhong light;
    light.setColor(glm::vec3(1.0f, 0.5f, 0.0f));
    light.setIntensity(0.8f);
    
    EXPECT_EQ(light.getColor(), glm::vec3(1.0f, 0.5f, 0.0f));
    EXPECT_FLOAT_EQ(light.getIntensity(), 0.8f);
}

TEST(DirectionalLightPhongTest, EnableDisable) {
    DirectionalLightPhong light;
    EXPECT_TRUE(light.isEnabled());
    
    light.setEnabled(false);
    EXPECT_FALSE(light.isEnabled());
    
    light.setEnabled(true);
    EXPECT_TRUE(light.isEnabled());
}

TEST(DirectionalLightPhongTest, GetShaderDataEnabled) {
    DirectionalLightPhong light("dir_light");
    light.setDirection(glm::vec3(0.0f, 1.0f, 0.0f));
    light.setColor(glm::vec3(1.0f, 0.0f, 0.0f));
    light.setIntensity(0.5f);
    light.setEnabled(true);
    
    LightShaderData data = light.getShaderData();
    
    EXPECT_EQ(data.position, glm::vec3(0.0f));
    EXPECT_EQ(data.direction, glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_EQ(data.color, glm::vec3(1.0f, 0.0f, 0.0f));
    EXPECT_FLOAT_EQ(data.intensity, 0.5f);
    EXPECT_FLOAT_EQ(data.constant, 1.0f);
    EXPECT_FLOAT_EQ(data.linear, 0.0f);
    EXPECT_FLOAT_EQ(data.quadratic, 0.0f);
    EXPECT_EQ(data.type, 0);
    EXPECT_EQ(data.enabled, 1);
}

TEST(DirectionalLightPhongTest, GetShaderDataDisabled) {
    DirectionalLightPhong light;
    light.setEnabled(false);
    
    LightShaderData data = light.getShaderData();
    EXPECT_EQ(data.enabled, 0);
}

TEST(DirectionalLightPhongTest, GetShaderDataNormalizedDirection) {
    DirectionalLightPhong light;
    light.setDirection(glm::vec3(5.0f, 0.0f, 0.0f));
    
    LightShaderData data = light.getShaderData();
    EXPECT_NEAR(data.direction.x, 1.0f, 0.0001f);
    EXPECT_NEAR(data.direction.y, 0.0f, 0.0001f);
    EXPECT_NEAR(data.direction.z, 0.0f, 0.0001f);
}

// ============== PointLightPhong Tests ==============

TEST(PointLightPhongTest, DefaultConstruction) {
    PointLightPhong light("test_point");
    
    EXPECT_EQ(light.getType(), LightTypePhong::Point);
    EXPECT_EQ(light.getName(), "test_point");
    EXPECT_TRUE(light.isEnabled());
    EXPECT_FLOAT_EQ(light.getIntensity(), 1.0f);
    EXPECT_EQ(light.getColor(), glm::vec3(1.0f));
    EXPECT_EQ(light.getPosition(), glm::vec3(0.0f));
    EXPECT_FLOAT_EQ(light.getConstant(), 1.0f);
    EXPECT_FLOAT_EQ(light.getLinear(), 0.09f);
    EXPECT_FLOAT_EQ(light.getQuadratic(), 0.032f);
}

TEST(PointLightPhongTest, DefaultConstructionEmptyName) {
    PointLightPhong light;
    EXPECT_EQ(light.getName(), "");
}

TEST(PointLightPhongTest, SetPosition) {
    PointLightPhong light;
    light.setPosition(glm::vec3(5.0f, 3.0f, -2.0f));
    
    EXPECT_EQ(light.getPosition(), glm::vec3(5.0f, 3.0f, -2.0f));
}

TEST(PointLightPhongTest, SetAttenuation) {
    PointLightPhong light;
    light.setAttenuation(1.0f, 0.045f, 0.0075f);
    
    EXPECT_FLOAT_EQ(light.getConstant(), 1.0f);
    EXPECT_FLOAT_EQ(light.getLinear(), 0.045f);
    EXPECT_FLOAT_EQ(light.getQuadratic(), 0.0075f);
}

TEST(PointLightPhongTest, SetColorAndIntensity) {
    PointLightPhong light;
    light.setColor(glm::vec3(0.0f, 1.0f, 0.5f));
    light.setIntensity(2.0f);
    
    EXPECT_EQ(light.getColor(), glm::vec3(0.0f, 1.0f, 0.5f));
    EXPECT_FLOAT_EQ(light.getIntensity(), 2.0f);
}

TEST(PointLightPhongTest, EnableDisable) {
    PointLightPhong light;
    EXPECT_TRUE(light.isEnabled());
    
    light.setEnabled(false);
    EXPECT_FALSE(light.isEnabled());
    
    light.setEnabled(true);
    EXPECT_TRUE(light.isEnabled());
}

TEST(PointLightPhongTest, GetShaderDataEnabled) {
    PointLightPhong light("point_light");
    light.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    light.setColor(glm::vec3(0.0f, 1.0f, 0.0f));
    light.setIntensity(2.0f);
    light.setAttenuation(1.0f, 0.1f, 0.05f);
    light.setEnabled(true);
    
    LightShaderData data = light.getShaderData();
    
    EXPECT_EQ(data.position, glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(data.direction, glm::vec3(0.0f));
    EXPECT_EQ(data.color, glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_FLOAT_EQ(data.intensity, 2.0f);
    EXPECT_FLOAT_EQ(data.constant, 1.0f);
    EXPECT_FLOAT_EQ(data.linear, 0.1f);
    EXPECT_FLOAT_EQ(data.quadratic, 0.05f);
    EXPECT_EQ(data.type, 1);
    EXPECT_EQ(data.enabled, 1);
}

TEST(PointLightPhongTest, GetShaderDataDisabled) {
    PointLightPhong light;
    light.setEnabled(false);
    
    LightShaderData data = light.getShaderData();
    EXPECT_EQ(data.enabled, 0);
}

TEST(PointLightPhongTest, GetShaderDataNegativePosition) {
    PointLightPhong light;
    light.setPosition(glm::vec3(-10.0f, -5.0f, -3.0f));
    
    LightShaderData data = light.getShaderData();
    EXPECT_EQ(data.position, glm::vec3(-10.0f, -5.0f, -3.0f));
}

// ============== PhongLightManager Tests ==============

TEST(PhongLightManagerTest, DefaultConstruction) {
    PhongLightManager manager;
    
    EXPECT_EQ(manager.getLightCount(), 0);
    EXPECT_EQ(manager.getEnabledLightCount(), 0);
    EXPECT_EQ(manager.getAmbientColor(), glm::vec3(0.1f, 0.1f, 0.1f));
}

TEST(PhongLightManagerTest, AddDirectionalLight) {
    PhongLightManager manager;
    
    auto light = std::make_shared<DirectionalLightPhong>("sun");
    manager.addDirectionalLight(light);
    
    EXPECT_EQ(manager.getLightCount(), 1);
    EXPECT_EQ(manager.getEnabledLightCount(), 1);
}

TEST(PhongLightManagerTest, AddPointLight) {
    PhongLightManager manager;
    
    auto light = std::make_shared<PointLightPhong>("point1");
    manager.addPointLight(light);
    
    EXPECT_EQ(manager.getLightCount(), 1);
    EXPECT_EQ(manager.getEnabledLightCount(), 1);
}

TEST(PhongLightManagerTest, AddMultipleLights) {
    PhongLightManager manager;
    
    manager.addDirectionalLight(std::make_shared<DirectionalLightPhong>("sun"));
    manager.addPointLight(std::make_shared<PointLightPhong>("point1"));
    manager.addPointLight(std::make_shared<PointLightPhong>("point2"));
    
    EXPECT_EQ(manager.getLightCount(), 3);
}

TEST(PhongLightManagerTest, MaxLights) {
    PhongLightManager manager;
    
    for (int idx = 0; idx < PhongLightManager::MAX_LIGHTS + 5; ++idx) {
        manager.addPointLight(std::make_shared<PointLightPhong>("light_" + std::to_string(idx)));
    }
    
    EXPECT_EQ(manager.getLightCount(), static_cast<size_t>(PhongLightManager::MAX_LIGHTS));
}

TEST(PhongLightManagerTest, GetLightByName) {
    PhongLightManager manager;
    
    auto sun = std::make_shared<DirectionalLightPhong>("sun");
    sun->setColor(glm::vec3(1.0f, 0.9f, 0.8f));
    manager.addDirectionalLight(sun);
    
    auto retrieved = manager.getLight("sun");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getColor(), glm::vec3(1.0f, 0.9f, 0.8f));
    
    auto notFound = manager.getLight("nonexistent");
    EXPECT_EQ(notFound, nullptr);
}

TEST(PhongLightManagerTest, RemoveLightByName) {
    PhongLightManager manager;
    
    manager.addDirectionalLight(std::make_shared<DirectionalLightPhong>("sun"));
    manager.addPointLight(std::make_shared<PointLightPhong>("point1"));
    
    EXPECT_EQ(manager.getLightCount(), 2);
    
    manager.removeLight("sun");
    EXPECT_EQ(manager.getLightCount(), static_cast<size_t>(1));
    EXPECT_EQ(manager.getLight("sun"), nullptr);
    EXPECT_NE(manager.getLight("point1"), nullptr);
}

TEST(PhongLightManagerTest, RemoveLightNonExistent) {
    PhongLightManager manager;
    
    manager.addPointLight(std::make_shared<PointLightPhong>("point1"));
    manager.removeLight("nonexistent");
    
    EXPECT_EQ(manager.getLightCount(), static_cast<size_t>(1));
}

TEST(PhongLightManagerTest, ClearLights) {
    PhongLightManager manager;
    
    manager.addDirectionalLight(std::make_shared<DirectionalLightPhong>("sun"));
    manager.addPointLight(std::make_shared<PointLightPhong>("point1"));
    manager.addPointLight(std::make_shared<PointLightPhong>("point2"));
    
    manager.clearLights();
    
    EXPECT_EQ(manager.getLightCount(), static_cast<size_t>(0));
    EXPECT_EQ(manager.getEnabledLightCount(), 0);
}

TEST(PhongLightManagerTest, GetDirectionalLights) {
    PhongLightManager manager;
    
    manager.addDirectionalLight(std::make_shared<DirectionalLightPhong>("dir1"));
    manager.addPointLight(std::make_shared<PointLightPhong>("point1"));
    manager.addPointLight(std::make_shared<PointLightPhong>("point2"));
    manager.addDirectionalLight(std::make_shared<DirectionalLightPhong>("dir2"));
    
    auto dirLights = manager.getDirectionalLights();
    EXPECT_EQ(dirLights.size(), static_cast<size_t>(2));
}

TEST(PhongLightManagerTest, GetPointLights) {
    PhongLightManager manager;
    
    manager.addDirectionalLight(std::make_shared<DirectionalLightPhong>("dir1"));
    manager.addPointLight(std::make_shared<PointLightPhong>("point1"));
    manager.addPointLight(std::make_shared<PointLightPhong>("point2"));
    
    auto pointLights = manager.getPointLights();
    EXPECT_EQ(pointLights.size(), static_cast<size_t>(2));
}

TEST(PhongLightManagerTest, GetDirectionalLightsEmpty) {
    PhongLightManager manager;
    
    manager.addPointLight(std::make_shared<PointLightPhong>("point1"));
    
    auto dirLights = manager.getDirectionalLights();
    EXPECT_EQ(dirLights.size(), static_cast<size_t>(0));
}

TEST(PhongLightManagerTest, GetPointLightsEmpty) {
    PhongLightManager manager;
    
    manager.addDirectionalLight(std::make_shared<DirectionalLightPhong>("dir1"));
    
    auto pointLights = manager.getPointLights();
    EXPECT_EQ(pointLights.size(), static_cast<size_t>(0));
}

TEST(PhongLightManagerTest, EnabledLightCount) {
    PhongLightManager manager;
    
    auto light1 = std::make_shared<PointLightPhong>("point1");
    auto light2 = std::make_shared<PointLightPhong>("point2");
    auto light3 = std::make_shared<PointLightPhong>("point3");
    
    manager.addPointLight(light1);
    manager.addPointLight(light2);
    manager.addPointLight(light3);
    
    EXPECT_EQ(manager.getEnabledLightCount(), 3);
    
    light2->setEnabled(false);
    EXPECT_EQ(manager.getEnabledLightCount(), 2);
    
    light1->setEnabled(false);
    light3->setEnabled(false);
    EXPECT_EQ(manager.getEnabledLightCount(), 0);
}

TEST(PhongLightManagerTest, ForEachEnabledLight) {
    PhongLightManager manager;
    
    auto light1 = std::make_shared<PointLightPhong>("point1");
    auto light2 = std::make_shared<PointLightPhong>("point2");
    light2->setEnabled(false);
    auto light3 = std::make_shared<PointLightPhong>("point3");
    
    manager.addPointLight(light1);
    manager.addPointLight(light2);
    manager.addPointLight(light3);
    
    int count = 0;
    manager.forEachEnabledLight([&count](const PhongLight* light, int index) {
        count++;
    });
    
    EXPECT_EQ(count, 2);
}

TEST(PhongLightManagerTest, ForEachEnabledLightIndices) {
    PhongLightManager manager;
    
    auto light1 = std::make_shared<PointLightPhong>("point1");
    auto light2 = std::make_shared<PointLightPhong>("point2");
    auto light3 = std::make_shared<PointLightPhong>("point3");
    light2->setEnabled(false);
    
    manager.addPointLight(light1);
    manager.addPointLight(light2);
    manager.addPointLight(light3);
    
    std::vector<int> indices;
    manager.forEachEnabledLight([&indices](const PhongLight* light, int index) {
        indices.push_back(index);
    });
    
    ASSERT_EQ(indices.size(), static_cast<size_t>(2));
    EXPECT_EQ(indices[0], 0);
    EXPECT_EQ(indices[1], 1);
}

TEST(PhongLightManagerTest, SetAmbientColor) {
    PhongLightManager manager;
    manager.setAmbientColor(glm::vec3(0.2f, 0.1f, 0.1f));
    
    EXPECT_EQ(manager.getAmbientColor(), glm::vec3(0.2f, 0.1f, 0.1f));
}

TEST(PhongLightManagerTest, GetShaderDataArrayEmpty) {
    PhongLightManager manager;
    
    auto arr = manager.getShaderDataArray();
    
    for (int i = 0; i < PhongLightManager::MAX_LIGHTS; ++i) {
        EXPECT_EQ(arr[i].enabled, 0);
    }
}

TEST(PhongLightManagerTest, GetShaderDataArrayWithLights) {
    PhongLightManager manager;
    
    auto dirLight = std::make_shared<DirectionalLightPhong>("sun");
    dirLight->setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
    dirLight->setColor(glm::vec3(1.0f, 1.0f, 0.9f));
    dirLight->setIntensity(1.0f);
    manager.addDirectionalLight(dirLight);
    
    auto pointLight = std::make_shared<PointLightPhong>("lamp");
    pointLight->setPosition(glm::vec3(2.0f, 3.0f, 1.0f));
    pointLight->setColor(glm::vec3(1.0f, 0.5f, 0.0f));
    pointLight->setIntensity(0.8f);
    pointLight->setAttenuation(1.0f, 0.07f, 0.017f);
    manager.addPointLight(pointLight);
    
    auto arr = manager.getShaderDataArray();
    
    EXPECT_EQ(arr[0].direction, glm::vec3(0.0f, -1.0f, 0.0f));
    EXPECT_EQ(arr[0].color, glm::vec3(1.0f, 1.0f, 0.9f));
    EXPECT_FLOAT_EQ(arr[0].intensity, 1.0f);
    EXPECT_EQ(arr[0].type, 0);
    EXPECT_EQ(arr[0].enabled, 1);
    
    EXPECT_EQ(arr[1].position, glm::vec3(2.0f, 3.0f, 1.0f));
    EXPECT_EQ(arr[1].color, glm::vec3(1.0f, 0.5f, 0.0f));
    EXPECT_FLOAT_EQ(arr[1].intensity, 0.8f);
    EXPECT_FLOAT_EQ(arr[1].constant, 1.0f);
    EXPECT_FLOAT_EQ(arr[1].linear, 0.07f);
    EXPECT_FLOAT_EQ(arr[1].quadratic, 0.017f);
    EXPECT_EQ(arr[1].type, 1);
    EXPECT_EQ(arr[1].enabled, 1);
    
    for (int i = 2; i < PhongLightManager::MAX_LIGHTS; ++i) {
        EXPECT_EQ(arr[i].enabled, 0);
    }
}

TEST(PhongLightManagerTest, GetShaderDataArrayWithDisabledLight) {
    PhongLightManager manager;
    
    auto light1 = std::make_shared<PointLightPhong>("enabled_light");
    light1->setEnabled(true);
    manager.addPointLight(light1);
    
    auto light2 = std::make_shared<PointLightPhong>("disabled_light");
    light2->setEnabled(false);
    manager.addPointLight(light2);
    
    auto arr = manager.getShaderDataArray();
    
    EXPECT_EQ(arr[0].enabled, 1);
    EXPECT_EQ(arr[1].enabled, 0);
}

TEST(PhongLightManagerTest, GetShaderDataArrayMaxLights) {
    PhongLightManager manager;
    
    for (int i = 0; i < PhongLightManager::MAX_LIGHTS; ++i) {
        auto light = std::make_shared<PointLightPhong>("light_" + std::to_string(i));
        light->setPosition(glm::vec3(static_cast<float>(i), 0.0f, 0.0f));
        manager.addPointLight(light);
    }
    
    auto arr = manager.getShaderDataArray();
    
    for (int i = 0; i < PhongLightManager::MAX_LIGHTS; ++i) {
        EXPECT_EQ(arr[i].enabled, 1);
        EXPECT_FLOAT_EQ(arr[i].position.x, static_cast<float>(i));
    }
}

// ============== PhongMaterial Tests ==============

TEST(PhongMaterialTest, DefaultConstruction) {
    PhongMaterial material;
    
    EXPECT_EQ(material.ambient, glm::vec3(0.1f));
    EXPECT_EQ(material.diffuse, glm::vec3(0.8f));
    EXPECT_EQ(material.specular, glm::vec3(1.0f));
    EXPECT_FLOAT_EQ(material.shininess, 32.0f);
}

TEST(PhongMaterialTest, ParameterizedConstruction) {
    PhongMaterial material(
        glm::vec3(0.2f, 0.1f, 0.1f),
        glm::vec3(0.6f, 0.3f, 0.2f),
        glm::vec3(0.9f, 0.8f, 0.7f),
        64.0f
    );
    
    EXPECT_EQ(material.ambient, glm::vec3(0.2f, 0.1f, 0.1f));
    EXPECT_EQ(material.diffuse, glm::vec3(0.6f, 0.3f, 0.2f));
    EXPECT_EQ(material.specular, glm::vec3(0.9f, 0.8f, 0.7f));
    EXPECT_FLOAT_EQ(material.shininess, 64.0f);
}

// ============== LightTypePhong Enum Tests ==============

TEST(LightTypePhongTest, EnumValues) {
    EXPECT_EQ(static_cast<int>(LightTypePhong::Directional), 0);
    EXPECT_EQ(static_cast<int>(LightTypePhong::Point), 1);
}

// ============== Integration Tests ==============

TEST(PhongLightIntegrationTest, MixedLightsInManager) {
    PhongLightManager manager;
    
    auto sun = std::make_shared<DirectionalLightPhong>("sun");
    sun->setDirection(glm::vec3(-0.5f, -1.0f, -0.3f));
    sun->setColor(glm::vec3(1.0f, 0.95f, 0.8f));
    sun->setIntensity(1.2f);
    
    auto lamp1 = std::make_shared<PointLightPhong>("lamp1");
    lamp1->setPosition(glm::vec3(3.0f, 2.0f, 1.0f));
    lamp1->setColor(glm::vec3(1.0f, 0.8f, 0.6f));
    lamp1->setIntensity(0.8f);
    lamp1->setAttenuation(1.0f, 0.09f, 0.032f);
    
    auto lamp2 = std::make_shared<PointLightPhong>("lamp2");
    lamp2->setPosition(glm::vec3(-2.0f, 1.5f, 4.0f));
    lamp2->setColor(glm::vec3(0.6f, 0.8f, 1.0f));
    lamp2->setIntensity(0.6f);
    lamp2->setEnabled(false);
    
    manager.addDirectionalLight(sun);
    manager.addPointLight(lamp1);
    manager.addPointLight(lamp2);
    
    EXPECT_EQ(manager.getLightCount(), static_cast<size_t>(3));
    EXPECT_EQ(manager.getEnabledLightCount(), 2);
    
    auto dirLights = manager.getDirectionalLights();
    auto pointLights = manager.getPointLights();
    
    EXPECT_EQ(dirLights.size(), static_cast<size_t>(1));
    EXPECT_EQ(pointLights.size(), static_cast<size_t>(2));
    
    auto arr = manager.getShaderDataArray();
    EXPECT_EQ(arr[0].type, 0);
    EXPECT_EQ(arr[1].type, 1);
    EXPECT_EQ(arr[2].type, 1);
    EXPECT_EQ(arr[2].enabled, 0);
}

TEST(PhongLightIntegrationTest, ModifyLightsAfterAdding) {
    PhongLightManager manager;
    
    auto light = std::make_shared<PointLightPhong>("test");
    light->setPosition(glm::vec3(0.0f));
    light->setColor(glm::vec3(1.0f));
    light->setIntensity(1.0f);
    manager.addPointLight(light);
    
    light->setPosition(glm::vec3(5.0f, 5.0f, 5.0f));
    light->setColor(glm::vec3(0.5f, 0.5f, 0.5f));
    light->setIntensity(0.5f);
    
    auto retrieved = manager.getLight("test");
    ASSERT_NE(retrieved, nullptr);
    
    auto pointLight = std::static_pointer_cast<PointLightPhong>(retrieved);
    EXPECT_EQ(pointLight->getPosition(), glm::vec3(5.0f, 5.0f, 5.0f));
    EXPECT_EQ(pointLight->getColor(), glm::vec3(0.5f, 0.5f, 0.5f));
    EXPECT_FLOAT_EQ(pointLight->getIntensity(), 0.5f);
}
