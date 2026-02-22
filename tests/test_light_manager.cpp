/**
 * @file test_light_manager.cpp
 * @brief LightManager unit tests
 */

#include <gtest/gtest.h>
#include "lighting/LightManager.h"
#include <string>

TEST(LightManagerTest, DefaultConstruction) {
    LightManager manager;
    
    EXPECT_EQ(manager.getLightCount(), 0);
    EXPECT_EQ(manager.getEnabledLightCount(), 0);
    EXPECT_EQ(manager.getAmbientColor(), glm::vec3(0.1f));
}

TEST(LightManagerTest, AddLight) {
    LightManager manager;
    
    auto light = std::make_shared<DirectionalLight>("sun");
    manager.addLight(light);
    
    EXPECT_EQ(manager.getLightCount(), 1);
    EXPECT_EQ(manager.getEnabledLightCount(), 1);
}

TEST(LightManagerTest, AddMultipleLights) {
    LightManager manager;
    
    manager.addLight(std::make_shared<DirectionalLight>("sun"));
    manager.addLight(std::make_shared<PointLight>("point1"));
    manager.addLight(std::make_shared<PointLight>("point2"));
    
    EXPECT_EQ(manager.getLightCount(), 3);
}

TEST(LightManagerTest, MaxLights) {
    LightManager manager;
    
    // Add more than MAX_LIGHTS
    for (int idx = 0; idx < LightManager::MAX_LIGHTS + 5; ++idx) {
        manager.addLight(std::make_shared<PointLight>("light_" + std::to_string(idx)));
    }
    
    // Should only keep MAX_LIGHTS
    EXPECT_EQ(manager.getLightCount(), static_cast<size_t>(LightManager::MAX_LIGHTS));
}

TEST(LightManagerTest, GetLightByName) {
    LightManager manager;
    
    auto sun = std::make_shared<DirectionalLight>("sun");
    sun->setColor(glm::vec3(1.0f, 0.9f, 0.8f));
    manager.addLight(sun);
    
    auto retrieved = manager.getLight("sun");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getColor(), glm::vec3(1.0f, 0.9f, 0.8f));
    
    auto notFound = manager.getLight("nonexistent");
    EXPECT_EQ(notFound, nullptr);
}

TEST(LightManagerTest, GetLightByIndex) {
    LightManager manager;
    
    manager.addLight(std::make_shared<DirectionalLight>("light0"));
    manager.addLight(std::make_shared<PointLight>("light1"));
    
    auto light0 = manager.getLight(0);
    auto light1 = manager.getLight(1);
    auto invalid = manager.getLight(100);
    
    ASSERT_NE(light0, nullptr);
    ASSERT_NE(light1, nullptr);
    EXPECT_EQ(light0->getName(), "light0");
    EXPECT_EQ(light1->getName(), "light1");
    EXPECT_EQ(invalid, nullptr);
}

TEST(LightManagerTest, RemoveLightByName) {
    LightManager manager;
    
    manager.addLight(std::make_shared<DirectionalLight>("sun"));
    manager.addLight(std::make_shared<PointLight>("point1"));
    
    EXPECT_EQ(manager.getLightCount(), 2);
    
    manager.removeLight("sun");
    EXPECT_EQ(manager.getLightCount(), static_cast<size_t>(1));
    EXPECT_EQ(manager.getLight("sun"), nullptr);
    EXPECT_NE(manager.getLight("point1"), nullptr);
}

TEST(LightManagerTest, RemoveLightByIndex) {
    LightManager manager;
    
    manager.addLight(std::make_shared<DirectionalLight>("light0"));
    manager.addLight(std::make_shared<PointLight>("light1"));
    manager.addLight(std::make_shared<SpotLight>("light2"));
    
    manager.removeLight(1); // Remove light1
    
    EXPECT_EQ(manager.getLightCount(), static_cast<size_t>(2));
    EXPECT_EQ(manager.getLight("light1"), nullptr);
    EXPECT_NE(manager.getLight("light0"), nullptr);
    EXPECT_NE(manager.getLight("light2"), nullptr);
}

TEST(LightManagerTest, ClearLights) {
    LightManager manager;
    
    manager.addLight(std::make_shared<DirectionalLight>("sun"));
    manager.addLight(std::make_shared<PointLight>("point1"));
    manager.addLight(std::make_shared<PointLight>("point2"));
    
    manager.clearLights();
    
    EXPECT_EQ(manager.getLightCount(), static_cast<size_t>(0));
    EXPECT_EQ(manager.getEnabledLightCount(), 0);
}

TEST(LightManagerTest, GetLightsByType) {
    LightManager manager;
    
    manager.addLight(std::make_shared<DirectionalLight>("dir1"));
    manager.addLight(std::make_shared<PointLight>("point1"));
    manager.addLight(std::make_shared<PointLight>("point2"));
    manager.addLight(std::make_shared<SpotLight>("spot1"));
    
    auto pointLights = manager.getPointLights();
    EXPECT_EQ(pointLights.size(), static_cast<size_t>(2));
    
    auto dirLights = manager.getDirectionalLights();
    EXPECT_EQ(dirLights.size(), static_cast<size_t>(1));
    
    auto spotLights = manager.getSpotLights();
    EXPECT_EQ(spotLights.size(), static_cast<size_t>(1));
}

TEST(LightManagerTest, EnabledLightCount) {
    LightManager manager;
    
    auto light1 = std::make_shared<PointLight>("point1");
    auto light2 = std::make_shared<PointLight>("point2");
    auto light3 = std::make_shared<PointLight>("point3");
    
    manager.addLight(light1);
    manager.addLight(light2);
    manager.addLight(light3);
    
    EXPECT_EQ(manager.getEnabledLightCount(), 3);
    
    light2->setEnabled(false);
    EXPECT_EQ(manager.getEnabledLightCount(), 2);
    
    light1->setEnabled(false);
    light3->setEnabled(false);
    EXPECT_EQ(manager.getEnabledLightCount(), 0);
}

TEST(LightManagerTest, ForEachEnabledLight) {
    LightManager manager;
    
    auto light1 = std::make_shared<PointLight>("point1");
    auto light2 = std::make_shared<PointLight>("point2");
    light2->setEnabled(false);
    auto light3 = std::make_shared<PointLight>("point3");
    
    manager.addLight(light1);
    manager.addLight(light2);
    manager.addLight(light3);
    
    int count = 0;
    manager.forEachEnabledLight([&count](const Light* light, int index) {
        count++;
    });
    
    EXPECT_EQ(count, 2);
}

TEST(LightManagerTest, SetAmbientColor) {
    LightManager manager;
    manager.setAmbientColor(glm::vec3(0.2f, 0.1f, 0.1f));
    
    EXPECT_EQ(manager.getAmbientColor(), glm::vec3(0.2f, 0.1f, 0.1f));
}
