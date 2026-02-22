/**
 * @file ShadowMapper.h
 * @brief Shadow mapping system using depth buffer
 */

#ifndef SHADOW_MAPPER_H
#define SHADOW_MAPPER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

/**
 * @brief Shadow map configuration
 */
struct ShadowConfig {
    int width = 1024;
    int height = 1024;
    float nearPlane = 1.0f;
    float farPlane = 25.0f;
    float orthoSize = 10.0f;
    bool pcfEnabled = true;
    int pcfKernelSize = 3;
    float biasMin = 0.001f;
    float biasMax = 0.01f;
};

/**
 * @brief Shadow Mapper class
 * 
 * Handles shadow map generation for directional lights.
 * Uses frame buffer with depth texture for shadow mapping.
 */
class ShadowMapper {
public:
    ShadowMapper();
    explicit ShadowMapper(const ShadowConfig& config);
    ~ShadowMapper();
    
    // Non-copyable
    ShadowMapper(const ShadowMapper&) = delete;
    ShadowMapper& operator=(const ShadowMapper&) = delete;
    
    /**
     * @brief Initialize shadow map resources
     * @return true if successful
     */
    bool initialize();
    
    /**
     * @brief Begin shadow map rendering pass
     * Call this before rendering scene from light's perspective
     */
    void beginPass();
    
    /**
     * @brief End shadow map rendering pass
     * Call this after rendering scene from light's perspective
     */
    void endPass();
    
    /**
     * @brief Bind shadow map texture for reading
     * @param textureUnit Texture unit to bind to
     */
    void bindShadowMap(unsigned int textureUnit = 0) const;
    
    /**
     * @brief Calculate light space matrix for directional light
     * @param lightDir Light direction
     * @param sceneCenter Center of the scene
     * @return Light space matrix
     */
    glm::mat4 calculateLightSpaceMatrix(const glm::vec3& lightDir, 
                                         const glm::vec3& sceneCenter = glm::vec3(0.0f)) const;
    
    /**
     * @brief Get current light space matrix
     * @return Light space matrix
     */
    glm::mat4 getLightSpaceMatrix() const { return lightSpaceMatrix_; }
    
    /**
     * @brief Update light space matrix
     * @param lightDir New light direction
     * @param sceneCenter Scene center for frustum calculation
     */
    void updateLightSpaceMatrix(const glm::vec3& lightDir, 
                                const glm::vec3& sceneCenter = glm::vec3(0.0f));
    
    // Getters
    unsigned int getDepthMapFBO() const { return depthMapFBO_; }
    unsigned int getDepthMapTexture() const { return depthMap_; }
    int getWidth() const { return config_.width; }
    int getHeight() const { return config_.height; }
    bool isPCFEnabled() const { return config_.pcfEnabled; }
    float getBiasMin() const { return config_.biasMin; }
    float getBiasMax() const { return config_.biasMax; }
    
    // Setters
    void setPCFEnabled(bool enabled) { config_.pcfEnabled = enabled; }
    void setPCFKernelSize(int size) { config_.pcfKernelSize = size; }
    void setBias(float minBias, float maxBias) {
        config_.biasMin = minBias;
        config_.biasMax = maxBias;
    }
    void setResolution(int width, int height);
    
private:
    ShadowConfig config_;
    unsigned int depthMapFBO_;
    unsigned int depthMap_;
    glm::mat4 lightSpaceMatrix_;
    bool initialized_;
    
    /**
     * @brief Create frame buffer and depth texture
     * @return true if successful
     */
    bool createFramebuffer();
};

#endif // SHADOW_MAPPER_H
