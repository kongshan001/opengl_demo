/**
 * @file ParticleRenderer.h
 * @brief Particle system renderer
 */

#ifndef PARTICLE_RENDERER_H
#define PARTICLE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include "particles/Particle.h"
#include "shader/Shader.h"

/**
 * @brief Particle system renderer
 * 
 * Handles rendering of particle systems with billboarding support.
 */
class ParticleRenderer {
public:
    ParticleRenderer();
    ~ParticleRenderer();
    
    /**
     * @brief Initialize renderer resources
     * @return true if successful
     */
    bool initialize();
    
    /**
     * @brief Render particles from an emitter
     * @param emitter Particle emitter to render
     * @param view View matrix
     * @param projection Projection matrix
     */
    void render(const ParticleEmitter& emitter, 
                const glm::mat4& view, 
                const glm::mat4& projection);
    
    /**
     * @brief Set particle texture
     * @param texturePath Path to texture file
     */
    void setTexture(const std::string& texturePath);
    
    /**
     * @brief Enable/disable additive blending
     * @param enabled Use additive blending
     */
    void setAdditiveBlending(bool enabled) { additiveBlending_ = enabled; }
    
private:
    std::shared_ptr<CShader> shader_;
    unsigned int vao_;
    unsigned int vbo_;
    unsigned int texture_;
    bool hasTexture_;
    bool additiveBlending_;
    bool initialized_;
    
    /**
     * @brief Create quad VAO for particle rendering
     */
    void createQuadVAO();
    
    /**
     * @brief Update particle buffer data
     * @param emitter Particle emitter
     */
    void updateParticleBuffer(const ParticleEmitter& emitter);
};

#endif // PARTICLE_RENDERER_H
