/**
 * @file LightManager.cpp
 * @brief Phong light manager implementation
 */

#include "light/LightManager.h"
#include <algorithm>

PhongLightManager::PhongLightManager()
    : ambientColor_(0.1f, 0.1f, 0.1f) {
}

void PhongLightManager::addDirectionalLight(std::shared_ptr<DirectionalLightPhong> light) {
    if (lights_.size() < MAX_LIGHTS) {
        lights_.push_back(light);
    }
}

void PhongLightManager::addPointLight(std::shared_ptr<PointLightPhong> light) {
    if (lights_.size() < MAX_LIGHTS) {
        lights_.push_back(light);
    }
}

void PhongLightManager::removeLight(const std::string& name) {
    auto it = std::find_if(lights_.begin(), lights_.end(),
        [&name](const std::shared_ptr<PhongLight>& l) {
            return l->getName() == name;
        });
    if (it != lights_.end()) {
        lights_.erase(it);
    }
}

void PhongLightManager::clearLights() {
    lights_.clear();
}

std::shared_ptr<PhongLight> PhongLightManager::getLight(const std::string& name) const {
    auto it = std::find_if(lights_.begin(), lights_.end(),
        [&name](const std::shared_ptr<PhongLight>& l) {
            return l->getName() == name;
        });
    return it != lights_.end() ? *it : nullptr;
}

std::vector<std::shared_ptr<DirectionalLightPhong>> PhongLightManager::getDirectionalLights() const {
    std::vector<std::shared_ptr<DirectionalLightPhong>> result;
    for (const auto& light : lights_) {
        if (light->getType() == LightTypePhong::Directional) {
            result.push_back(std::static_pointer_cast<DirectionalLightPhong>(light));
        }
    }
    return result;
}

std::vector<std::shared_ptr<PointLightPhong>> PhongLightManager::getPointLights() const {
    std::vector<std::shared_ptr<PointLightPhong>> result;
    for (const auto& light : lights_) {
        if (light->getType() == LightTypePhong::Point) {
            result.push_back(std::static_pointer_cast<PointLightPhong>(light));
        }
    }
    return result;
}

int PhongLightManager::getEnabledLightCount() const {
    int count = 0;
    for (const auto& light : lights_) {
        if (light->isEnabled()) ++count;
    }
    return count;
}

void PhongLightManager::forEachEnabledLight(std::function<void(const PhongLight*, int)> callback) const {
    int index = 0;
    for (const auto& light : lights_) {
        if (light->isEnabled()) {
            callback(light.get(), index++);
        }
    }
}

std::array<LightShaderData, PhongLightManager::MAX_LIGHTS> PhongLightManager::getShaderDataArray() const {
    std::array<LightShaderData, MAX_LIGHTS> arr;
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        arr[i].enabled = 0;
    }
    int idx = 0;
    for (const auto& light : lights_) {
        if (idx >= MAX_LIGHTS) break;
        arr[idx++] = light->getShaderData();
    }
    return arr;
}
