/**
 * @file Particle.cpp
 * @brief Particle system implementation
 */

#include "particles/Particle.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

// Random helper functions
namespace {
    float randomFloat(float min, float max) {
        return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
    }
    
    float randomVariance(float base, float variance) {
        return base + randomFloat(-variance, variance);
    }
    
    glm::vec3 randomVec3Variance(const glm::vec3& base, const glm::vec3& variance) {
        return glm::vec3(
            randomVariance(base.x, variance.x),
            randomVariance(base.y, variance.y),
            randomVariance(base.z, variance.z)
        );
    }
}

// ==================== ParticleEmitter ====================

ParticleEmitter::ParticleEmitter()
    : firstDead_(0)
    , emissionAccumulator_(0.0f)
    , enabled_(true) {
    particles_.resize(config_.maxParticles);
}

ParticleEmitter::ParticleEmitter(const ParticleEmitterConfig& config)
    : config_(config)
    , firstDead_(0)
    , emissionAccumulator_(0.0f)
    , enabled_(true) {
    particles_.resize(config_.maxParticles);
}

void ParticleEmitter::update(float deltaTime) {
    if (!enabled_) return;
    
    // Emit new particles based on emission rate
    emissionAccumulator_ += deltaTime * config_.emissionRate;
    while (emissionAccumulator_ >= 1.0f) {
        emit(1);
        emissionAccumulator_ -= 1.0f;
    }
    
    // Update existing particles
    for (auto& particle : particles_) {
        if (particle.isAlive()) {
            // Apply gravity and drag
            particle.velocity += config_.gravity * deltaTime;
            particle.velocity *= (1.0f - config_.drag * deltaTime);
            
            // Update position
            particle.position += particle.velocity * deltaTime;
            
            // Update rotation
            particle.rotation += particle.rotationSpeed * deltaTime;
            
            // Update life
            particle.life -= deltaTime;
            
            // Interpolate color and scale based on life ratio
            float t = 1.0f - particle.getLifeRatio();  // 0 = new, 1 = dying
            particle.color = mix(config_.colorStart, config_.colorEnd, t);
            particle.scale = mix(config_.scaleStart, config_.scaleEnd, t);
        }
    }
}

void ParticleEmitter::emit(int count) {
    for (int i = 0; i < count; ++i) {
        int idx = findDeadParticle();
        if (idx < 0) break;  // No dead particles available
        
        initParticle(particles_[idx]);
    }
}

void ParticleEmitter::burst() {
    int count = config_.burstCount > 0 ? config_.burstCount : config_.maxParticles;
    emit(count);
}

void ParticleEmitter::initParticle(Particle& particle) {
    particle.position = randomVec3Variance(config_.position, config_.positionVariance);
    particle.velocity = randomVec3Variance(config_.velocity, config_.velocityVariance);
    particle.life = randomFloat(config_.lifeMin, config_.lifeMax);
    particle.maxLife = particle.life;
    particle.rotation = randomFloat(config_.rotationMin, config_.rotationMax);
    particle.rotationSpeed = randomFloat(config_.rotationSpeedMin, config_.rotationSpeedMax);
    particle.color = config_.colorStart;
    particle.scale = config_.scaleStart;
}

int ParticleEmitter::findDeadParticle() {
    // Start from last known dead particle
    for (int i = firstDead_; i < static_cast<int>(particles_.size()); ++i) {
        if (!particles_[i].isAlive()) {
            firstDead_ = i;
            return i;
        }
    }
    
    // Search from beginning
    for (int i = 0; i < firstDead_; ++i) {
        if (!particles_[i].isAlive()) {
            firstDead_ = i;
            return i;
        }
    }
    
    return -1;  // All particles alive
}

std::vector<Particle*> ParticleEmitter::getAliveParticles() {
    std::vector<Particle*> alive;
    for (auto& particle : particles_) {
        if (particle.isAlive()) {
            alive.push_back(&particle);
        }
    }
    return alive;
}

int ParticleEmitter::getAliveCount() const {
    int count = 0;
    for (const auto& particle : particles_) {
        if (particle.isAlive()) count++;
    }
    return count;
}

void ParticleEmitter::setColors(const glm::vec4& start, const glm::vec4& end) {
    config_.colorStart = start;
    config_.colorEnd = end;
}

void ParticleEmitter::setScales(const glm::vec3& start, const glm::vec3& end) {
    config_.scaleStart = start;
    config_.scaleEnd = end;
}

void ParticleEmitter::reset() {
    for (auto& particle : particles_) {
        particle.life = 0.0f;
    }
    firstDead_ = 0;
    emissionAccumulator_ = 0.0f;
}

// ==================== ParticlePresets ====================

namespace ParticlePresets {

ParticleEmitterConfig createFire() {
    ParticleEmitterConfig config;
    config.emissionRate = 50.0f;
    config.velocity = glm::vec3(0.0f, 3.0f, 0.0f);
    config.velocityVariance = glm::vec3(0.5f, 1.0f, 0.5f);
    config.lifeMin = 0.5f;
    config.lifeMax = 1.5f;
    config.colorStart = glm::vec4(1.0f, 0.8f, 0.2f, 1.0f);
    config.colorEnd = glm::vec4(1.0f, 0.2f, 0.0f, 0.0f);
    config.scaleStart = glm::vec3(0.3f);
    config.scaleEnd = glm::vec3(0.05f);
    config.gravity = glm::vec3(0.0f, 1.0f, 0.0f);  // Fire rises
    config.drag = 0.2f;
    config.additiveBlending = true;
    return config;
}

ParticleEmitterConfig createSmoke() {
    ParticleEmitterConfig config;
    config.emissionRate = 20.0f;
    config.velocity = glm::vec3(0.0f, 1.5f, 0.0f);
    config.velocityVariance = glm::vec3(0.3f, 0.5f, 0.3f);
    config.lifeMin = 2.0f;
    config.lifeMax = 4.0f;
    config.colorStart = glm::vec4(0.3f, 0.3f, 0.3f, 0.6f);
    config.colorEnd = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);
    config.scaleStart = glm::vec3(0.2f);
    config.scaleEnd = glm::vec3(0.8f);
    config.gravity = glm::vec3(0.0f, 0.5f, 0.0f);
    config.drag = 0.3f;
    config.additiveBlending = false;
    return config;
}

ParticleEmitterConfig createRain() {
    ParticleEmitterConfig config;
    config.emissionRate = 200.0f;
    config.positionVariance = glm::vec3(10.0f, 0.0f, 10.0f);
    config.velocity = glm::vec3(0.0f, -15.0f, 0.0f);
    config.velocityVariance = glm::vec3(0.5f, 2.0f, 0.5f);
    config.lifeMin = 1.0f;
    config.lifeMax = 2.0f;
    config.colorStart = glm::vec4(0.6f, 0.7f, 0.9f, 0.5f);
    config.colorEnd = glm::vec4(0.4f, 0.5f, 0.7f, 0.3f);
    config.scaleStart = glm::vec3(0.02f, 0.15f, 0.02f);
    config.scaleEnd = glm::vec3(0.02f, 0.15f, 0.02f);
    config.gravity = glm::vec3(0.0f);
    config.drag = 0.0f;
    config.additiveBlending = true;
    return config;
}

ParticleEmitterConfig createSnow() {
    ParticleEmitterConfig config;
    config.emissionRate = 30.0f;
    config.positionVariance = glm::vec3(10.0f, 0.0f, 10.0f);
    config.velocity = glm::vec3(0.0f, -1.0f, 0.0f);
    config.velocityVariance = glm::vec3(0.5f, 0.2f, 0.5f);
    config.lifeMin = 5.0f;
    config.lifeMax = 10.0f;
    config.colorStart = glm::vec4(1.0f, 1.0f, 1.0f, 0.8f);
    config.colorEnd = glm::vec4(0.9f, 0.95f, 1.0f, 0.0f);
    config.scaleStart = glm::vec3(0.1f);
    config.scaleEnd = glm::vec3(0.05f);
    config.gravity = glm::vec3(0.0f, -0.5f, 0.0f);
    config.drag = 0.5f;
    config.additiveBlending = true;
    return config;
}

ParticleEmitterConfig createExplosion() {
    ParticleEmitterConfig config;
    config.emissionRate = 0.0f;  // Burst only
    config.burstCount = 200;
    config.velocity = glm::vec3(0.0f);
    config.velocityVariance = glm::vec3(10.0f);
    config.lifeMin = 0.5f;
    config.lifeMax = 1.5f;
    config.colorStart = glm::vec4(1.0f, 0.9f, 0.3f, 1.0f);
    config.colorEnd = glm::vec4(1.0f, 0.3f, 0.0f, 0.0f);
    config.scaleStart = glm::vec3(0.4f);
    config.scaleEnd = glm::vec3(0.02f);
    config.gravity = glm::vec3(0.0f, -5.0f, 0.0f);
    config.drag = 0.5f;
    config.additiveBlending = true;
    return config;
}

ParticleEmitterConfig createSparkle() {
    ParticleEmitterConfig config;
    config.emissionRate = 40.0f;
    config.velocity = glm::vec3(0.0f, 2.0f, 0.0f);
    config.velocityVariance = glm::vec3(1.0f, 1.0f, 1.0f);
    config.lifeMin = 0.3f;
    config.lifeMax = 0.8f;
    config.colorStart = glm::vec4(1.0f, 1.0f, 0.8f, 1.0f);
    config.colorEnd = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    config.scaleStart = glm::vec3(0.1f);
    config.scaleEnd = glm::vec3(0.01f);
    config.gravity = glm::vec3(0.0f);
    config.drag = 0.1f;
    config.additiveBlending = true;
    return config;
}

} // namespace ParticlePresets
