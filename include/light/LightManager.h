/**
 * @file LightManager.h
 * @brief Phong Light Manager - manages multiple point lights and directional lights
 */

#ifndef LIGHT_MANAGER_PHONG_H
#define LIGHT_MANAGER_PHONG_H

#include "light/Light.h"
#include <vector>
#include <memory>
#include <functional>

class PhongLightManager {
public:
    static constexpr int MAX_LIGHTS = 8;

    PhongLightManager();
    ~PhongLightManager() = default;

    void addDirectionalLight(std::shared_ptr<DirectionalLightPhong> light);
    void addPointLight(std::shared_ptr<PointLightPhong> light);
    void removeLight(const std::string& name);
    void clearLights();

    std::shared_ptr<PhongLight> getLight(const std::string& name) const;
    size_t getLightCount() const { return lights_.size(); }

    std::vector<std::shared_ptr<DirectionalLightPhong>> getDirectionalLights() const;
    std::vector<std::shared_ptr<PointLightPhong>> getPointLights() const;

    void setAmbientColor(const glm::vec3& color) { ambientColor_ = color; }
    glm::vec3 getAmbientColor() const { return ambientColor_; }

    int getEnabledLightCount() const;
    void forEachEnabledLight(std::function<void(const PhongLight*, int)> callback) const;

    std::array<LightShaderData, MAX_LIGHTS> getShaderDataArray() const;

private:
    std::vector<std::shared_ptr<PhongLight>> lights_;
    glm::vec3 ambientColor_;
};

#endif
