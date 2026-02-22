/**
 * @file LightManager.h
 * @brief Light Manager - manages all lights in the scene
 */

#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include "lighting/Light.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

// Windows compatibility
#ifdef _WIN32
    #define NOMINMAX
    #ifdef min
        #undef min
    #endif
    #ifdef max
        #undef max
    #endif
#endif

/**
 * @brief Light Manager
 * 
 * Manages all lights in the scene, supports up to 8 dynamic lights
 */
class LightManager {
public:
    static constexpr int MAX_LIGHTS = 8;

    LightManager();
    ~LightManager() = default;

    // Light management
    void addLight(std::shared_ptr<Light> light);
    void removeLight(const std::string& name);
    void removeLight(size_t index);
    void clearLights();

    std::shared_ptr<Light> getLight(const std::string& name) const;
    std::shared_ptr<Light> getLight(size_t index) const;
    size_t getLightCount() const { return lights_.size(); }

    // Light lookup
    std::vector<std::shared_ptr<Light>> getLightsByType(LightType type) const;
    std::vector<std::shared_ptr<PointLight>> getPointLights() const;
    std::vector<std::shared_ptr<DirectionalLight>> getDirectionalLights() const;
    std::vector<std::shared_ptr<SpotLight>> getSpotLights() const;

    // Global ambient light
    void setAmbientColor(const glm::vec3& color) { ambientColor_ = color; }
    glm::vec3 getAmbientColor() const { return ambientColor_; }

    // Iterate all enabled lights
    void forEachEnabledLight(std::function<void(const Light*, int)> callback) const;

    // Get enabled light count for shaders
    int getEnabledLightCount() const;

private:
    std::vector<std::shared_ptr<Light>> lights_;
    std::unordered_map<std::string, size_t> nameIndexMap_;
    glm::vec3 ambientColor_;
};

#endif // LIGHT_MANAGER_H
