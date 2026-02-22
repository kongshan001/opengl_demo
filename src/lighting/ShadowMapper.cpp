/**
 * @file ShadowMapper.cpp
 * @brief Shadow mapping system implementation
 */

#include "lighting/ShadowMapper.h"
#include <iostream>

ShadowMapper::ShadowMapper()
    : depthMapFBO_(0)
    , depthMap_(0)
    , lightSpaceMatrix_(1.0f)
    , initialized_(false) {
}

ShadowMapper::ShadowMapper(const ShadowConfig& config)
    : config_(config)
    , depthMapFBO_(0)
    , depthMap_(0)
    , lightSpaceMatrix_(1.0f)
    , initialized_(false) {
}

ShadowMapper::~ShadowMapper() {
    if (depthMapFBO_ != 0) {
        glDeleteFramebuffers(1, &depthMapFBO_);
    }
    if (depthMap_ != 0) {
        glDeleteTextures(1, &depthMap_);
    }
}

bool ShadowMapper::initialize() {
    if (initialized_) return true;
    
    if (!createFramebuffer()) {
        return false;
    }
    
    initialized_ = true;
    return true;
}

bool ShadowMapper::createFramebuffer() {
    // Create depth texture
    glGenTextures(1, &depthMap_);
    glBindTexture(GL_TEXTURE_2D, depthMap_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 config_.width, config_.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Wrap to border color to prevent shadows outside light frustum
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    // Create framebuffer
    glGenFramebuffers(1, &depthMapFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_, 0);
    
    // No color output
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Shadow map framebuffer incomplete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void ShadowMapper::beginPass() {
    // Save current viewport
    glViewport(0, 0, config_.width, config_.height);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Disable color writes (only depth matters)
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
}

void ShadowMapper::endPass() {
    // Re-enable color writes
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapper::bindShadowMap(unsigned int textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, depthMap_);
}

glm::mat4 ShadowMapper::calculateLightSpaceMatrix(const glm::vec3& lightDir, 
                                                    const glm::vec3& sceneCenter) const {
    // Light projection (orthographic for directional light)
    float halfSize = config_.orthoSize;
    glm::mat4 lightProjection = glm::ortho(-halfSize, halfSize, 
                                            -halfSize, halfSize,
                                            config_.nearPlane, config_.farPlane);
    
    // Light view (looking at scene center from light direction)
    glm::vec3 lightPos = sceneCenter - lightDir * (config_.farPlane * 0.5f);
    glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
    
    return lightProjection * lightView;
}

void ShadowMapper::updateLightSpaceMatrix(const glm::vec3& lightDir, 
                                          const glm::vec3& sceneCenter) {
    lightSpaceMatrix_ = calculateLightSpaceMatrix(lightDir, sceneCenter);
}

void ShadowMapper::setResolution(int width, int height) {
    config_.width = width;
    config_.height = height;
    
    if (initialized_) {
        // Recreate depth texture with new resolution
        glBindTexture(GL_TEXTURE_2D, depthMap_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                     config_.width, config_.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
}
