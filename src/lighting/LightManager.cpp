/**
 * @file LightManager.cpp
 * @brief Light manager implementation
 */

#include "lighting/LightManager.h"
#include <algorithm>

LightManager::LightManager()
    : ambientColor_(0.1f, 0.1f, 0.1f) {
}

void LightManager::addLight(std::shared_ptr<Light> light) {
    if (lights_.size() >= MAX_LIGHTS) {
        return; // Exceeded max lights
    }

    if (!light->getName().empty()) {
        nameIndexMap_[light->getName()] = lights_.size();
    }

    lights_.push_back(light);
}

void LightManager::removeLight(const std::string& name) {
    auto it = nameIndexMap_.find(name);
    if (it != nameIndexMap_.end()) {
        removeLight(it->second);
    }
}

void LightManager::removeLight(size_t index) {
    if (index >= lights_.size()) return;

    // Remove from name map
    const auto& light = lights_[index];
    if (!light->getName().empty()) {
        nameIndexMap_.erase(light->getName());
    }

    // Remove from list
    lights_.erase(lights_.begin() + index);

    // Rebuild name index map
    nameIndexMap_.clear();
    for (size_t i = 0; i < lights_.size(); ++i) {
        if (!lights_[i]->getName().empty()) {
            nameIndexMap_[lights_[i]->getName()] = i;
        }
    }
}

void LightManager::clearLights() {
    lights_.clear();
    nameIndexMap_.clear();
}

std::shared_ptr<Light> LightManager::getLight(const std::string& name) const {
    auto it = nameIndexMap_.find(name);
    if (it != nameIndexMap_.end() && it->second < lights_.size()) {
        return lights_[it->second];
    }
    return nullptr;
}

std::shared_ptr<Light> LightManager::getLight(size_t index) const {
    if (index < lights_.size()) {
        return lights_[index];
    }
    return nullptr;
}

std::vector<std::shared_ptr<Light>> LightManager::getLightsByType(LightType type) const {
    std::vector<std::shared_ptr<Light>> result;
    for (const auto& light : lights_) {
        if (light->getType() == type) {
            result.push_back(light);
        }
    }
    return result;
}

std::vector<std::shared_ptr<PointLight>> LightManager::getPointLights() const {
    std::vector<std::shared_ptr<PointLight>> result;
    for (const auto& light : lights_) {
        if (light->getType() == LightType::Point) {
            result.push_back(std::static_pointer_cast<PointLight>(light));
        }
    }
    return result;
}

std::vector<std::shared_ptr<DirectionalLight>> LightManager::getDirectionalLights() const {
    std::vector<std::shared_ptr<DirectionalLight>> result;
    for (const auto& light : lights_) {
        if (light->getType() == LightType::Directional) {
            result.push_back(std::static_pointer_cast<DirectionalLight>(light));
        }
    }
    return result;
}

std::vector<std::shared_ptr<SpotLight>> LightManager::getSpotLights() const {
    std::vector<std::shared_ptr<SpotLight>> result;
    for (const auto& light : lights_) {
        if (light->getType() == LightType::Spotlight) {
            result.push_back(std::static_pointer_cast<SpotLight>(light));
        }
    }
    return result;
}

void LightManager::forEachEnabledLight(std::function<void(const Light*, int)> callback) const {
    int index = 0;
    for (const auto& light : lights_) {
        if (light->isEnabled()) {
            callback(light.get(), index);
            ++index;
        }
    }
}

int LightManager::getEnabledLightCount() const {
    int count = 0;
    for (const auto& light : lights_) {
        if (light->isEnabled()) {
            ++count;
        }
    }
    return count;
}
