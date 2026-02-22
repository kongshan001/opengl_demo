/**
 * @file Skybox.h
 * @brief Skybox rendering with cube map
 */

#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

/**
 * @brief Skybox class
 * 
 * Renders a skybox using cubemap texture.
 * The skybox is always rendered at maximum depth (far plane).
 */
class Skybox {
public:
    Skybox();
    ~Skybox();
    
    // Non-copyable
    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;
    
    /**
     * @brief Initialize skybox resources
     * @return true if successful
     */
    bool initialize();
    
    /**
     * @brief Load skybox from cubemap faces
     * @param faces Array of 6 cubemap face textures (right, left, top, bottom, back, front)
     * @return true if successful
     */
    bool loadCubemap(const std::vector<std::string>& faces);
    
    /**
     * @brief Render skybox
     * @param view View matrix
     * @param projection Projection matrix
     */
    void render(const glm::mat4& view, const glm::mat4& projection);
    
    /**
     * @brief Enable/disable skybox
     * @param enabled Enable skybox rendering
     */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    /**
     * @brief Get cubemap texture ID
     * @return OpenGL texture ID
     */
    unsigned int getCubemapTexture() const { return cubemapTexture_; }
    
    /**
     * @brief Set skybox rotation (for day/night cycle)
     * @param yaw Rotation around Y axis (degrees)
     * @param pitch Rotation around X axis (degrees)
     */
    void setRotation(float yaw, float pitch);
    
    glm::vec2 getRotation() const { return glm::vec2(yaw_, pitch_); }
    
private:
    unsigned int vao_;
    unsigned int vbo_;
    unsigned int cubemapTexture_;
    bool enabled_;
    
    // Rotation for day/night cycle
    float yaw_;
    float pitch_;
    
    /**
     * @brief Create skybox VAO/VBO
     * @return true if successful
     */
    bool createBuffers();
};

/**
 * @brief Skybox presets
 * 
 * Preset skybox configurations
 */
namespace SkyboxPresets {
    std::vector<std::string> createDay();
    std::vector<std::string> createNight();
    std::vector<std::string> createSunset();
    std::vector<std::string> createCloudy();
}

#endif // SKYBOX_H
