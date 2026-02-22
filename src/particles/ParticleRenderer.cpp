/**
 * @file ParticleRenderer.cpp
 * @brief Particle system renderer implementation
 */

#include "particles/ParticleRenderer.h"
#include <vector>
#include <iostream>

ParticleRenderer::ParticleRenderer()
    : vao_(0)
    , vbo_(0)
    , texture_(0)
    , hasTexture_(false)
    , additiveBlending_(true)
    , initialized_(false) {
}

ParticleRenderer::~ParticleRenderer() {
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
    if (texture_ != 0) {
        glDeleteTextures(1, &texture_);
    }
}

bool ParticleRenderer::initialize() {
    if (initialized_) return true;
    
    // Load shader
    try {
        shader_ = std::make_shared<CShader>(
            std::string("resources/shaders/particle.vs"),
            std::string("resources/shaders/particle.fs")
        );
    } catch (const ShaderException& e) {
        std::cerr << "Particle shader error: " << e.what() << std::endl;
        return false;
    }
    
    createQuadVAO();
    initialized_ = true;
    return true;
}

void ParticleRenderer::createQuadVAO() {
    // We'll use instanced rendering with a dynamic buffer
    // Each particle: position(3) + color(4) + scale(3) + rotation(1) = 11 floats
    
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    
    // Allocate buffer (will be updated each frame)
    glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(float) * 11, nullptr, GL_DYNAMIC_DRAW);
    
    // Position attribute (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glVertexAttribDivisor(0, 1);  // Instance data
    
    // Color attribute (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribDivisor(1, 1);
    
    // Scale attribute (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(7 * sizeof(float)));
    glVertexAttribDivisor(2, 1);
    
    // Rotation attribute (location 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(10 * sizeof(float)));
    glVertexAttribDivisor(3, 1);
    
    glBindVertexArray(0);
}

void ParticleRenderer::updateParticleBuffer(const ParticleEmitter& emitter) {
    auto aliveParticles = const_cast<ParticleEmitter&>(emitter).getAliveParticles();
    if (aliveParticles.empty()) return;
    
    std::vector<float> data;
    data.reserve(aliveParticles.size() * 11);
    
    for (const Particle* p : aliveParticles) {
        // Position
        data.push_back(p->position.x);
        data.push_back(p->position.y);
        data.push_back(p->position.z);
        
        // Color
        data.push_back(p->color.r);
        data.push_back(p->color.g);
        data.push_back(p->color.b);
        data.push_back(p->color.a);
        
        // Scale
        data.push_back(p->scale.x);
        data.push_back(p->scale.y);
        data.push_back(p->scale.z);
        
        // Rotation
        data.push_back(p->rotation);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), data.data());
}

void ParticleRenderer::render(const ParticleEmitter& emitter, 
                               const glm::mat4& view, 
                               const glm::mat4& projection) {
    if (!initialized_ || !shader_) return;
    
    auto aliveParticles = const_cast<ParticleEmitter&>(emitter).getAliveParticles();
    if (aliveParticles.empty()) return;
    
    // Update buffer
    updateParticleBuffer(emitter);
    
    // Enable blending
    glEnable(GL_BLEND);
    if (additiveBlending_ || emitter.getConfig().additiveBlending) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    } else {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    // Disable depth writing (particles shouldn't block each other)
    glDepthMask(GL_FALSE);
    
    // Use shader
    shader_->use();
    shader_->setMat4("view", view);
    shader_->setMat4("projection", projection);
    
    // Calculate camera vectors for billboarding from view matrix
    glm::mat4 viewInverse = glm::inverse(view);
    glm::vec3 cameraRight = glm::vec3(viewInverse[0]);
    glm::vec3 cameraUp = glm::vec3(viewInverse[1]);
    shader_->setVec3("cameraRight", cameraRight);
    shader_->setVec3("cameraUp", cameraUp);
    
    // Bind texture if available
    if (hasTexture_ && texture_ != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_);
        shader_->setInt("particleTexture", 0);
        shader_->setInt("hasTexture", 1);
    } else {
        shader_->setInt("hasTexture", 0);
    }
    
    // Render particles as quads (4 vertices per particle, using instancing)
    glBindVertexArray(vao_);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(aliveParticles.size()));
    glBindVertexArray(0);
    
    // Restore state
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleRenderer::setTexture(const std::string& texturePath) {
    // Simple texture loading (could use CTexture class)
    if (texturePath.empty()) {
        hasTexture_ = false;
        return;
    }
    
    if (texture_ == 0) {
        glGenTextures(1, &texture_);
    }
    
    // For now, just mark as having texture
    // Full implementation would load the texture
    hasTexture_ = true;
}
