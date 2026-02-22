/**
 * @file Particle.h
 * @brief Particle structure and emitter system
 */

#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>
#include <vector>
#include <memory>

/**
 * @brief Single particle data
 */
struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;        // RGBA
    glm::vec3 scale;
    float life;             // Remaining life in seconds
    float maxLife;          // Total lifespan
    float rotation;
    float rotationSpeed;
    
    Particle()
        : position(0.0f)
        , velocity(0.0f)
        , color(1.0f)
        , scale(1.0f)
        , life(0.0f)
        , maxLife(1.0f)
        , rotation(0.0f)
        , rotationSpeed(0.0f)
    {}
    
    bool isAlive() const { return life > 0.0f; }
    float getLifeRatio() const { return life / maxLife; }
};

/**
 * @brief Particle emitter configuration
 */
struct ParticleEmitterConfig {
    // Emission
    int maxParticles = 1000;
    float emissionRate = 100.0f;    // Particles per second
    int burstCount = 0;              // Emit all at once (0 = continuous)
    
    // Position
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 positionVariance = glm::vec3(0.5f);
    
    // Velocity
    glm::vec3 velocity = glm::vec3(0.0f, 2.0f, 0.0f);
    glm::vec3 velocityVariance = glm::vec3(1.0f);
    
    // Life
    float lifeMin = 1.0f;
    float lifeMax = 2.0f;
    
    // Color (start -> end interpolation)
    glm::vec4 colorStart = glm::vec4(1.0f, 0.8f, 0.0f, 1.0f);  // Orange
    glm::vec4 colorEnd = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);    // Red, faded
    
    // Scale
    glm::vec3 scaleStart = glm::vec3(0.2f);
    glm::vec3 scaleEnd = glm::vec3(0.05f);
    
    // Physics
    glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
    float drag = 0.1f;
    
    // Rotation
    float rotationMin = 0.0f;
    float rotationMax = 360.0f;
    float rotationSpeedMin = -90.0f;
    float rotationSpeedMax = 90.0f;
    
    // Rendering
    bool additiveBlending = true;
    bool depthWrite = false;
    
    // Remove texturePath (handled externally)
};

/**
 * @brief Particle emitter class
 * 
 * Manages particle lifecycle, emission, and updates.
 */
class ParticleEmitter {
public:
    ParticleEmitter();
    explicit ParticleEmitter(const ParticleEmitterConfig& config);
    ~ParticleEmitter() = default;
    
    /**
     * @brief Update all particles
     * @param deltaTime Frame delta time
     */
    void update(float deltaTime);
    
    /**
     * @brief Emit particles (called automatically based on emission rate)
     * @param count Number of particles to emit
     */
    void emit(int count = 1);
    
    /**
     * @brief Emit burst of particles
     */
    void burst();
    
    /**
     * @brief Get all alive particles
     * @return Vector of particle pointers
     */
    std::vector<Particle*> getAliveParticles();
    
    /**
     * @brief Get all particles (including dead)
     * @return Reference to particle vector
     */
    const std::vector<Particle>& getAllParticles() const { return particles_; }
    
    /**
     * @brief Get count of alive particles
     * @return Number of alive particles
     */
    int getAliveCount() const;
    
    // Getters/Setters
    const ParticleEmitterConfig& getConfig() const { return config_; }
    void setConfig(const ParticleEmitterConfig& config) { config_ = config; }
    
    void setPosition(const glm::vec3& pos) { config_.position = pos; }
    glm::vec3 getPosition() const { return config_.position; }
    
    void setVelocity(const glm::vec3& vel) { config_.velocity = vel; }
    void setColors(const glm::vec4& start, const glm::vec4& end);
    void setScales(const glm::vec3& start, const glm::vec3& end);
    
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    void reset();
    
private:
    ParticleEmitterConfig config_;
    std::vector<Particle> particles_;
    int firstDead_;
    float emissionAccumulator_;
    bool enabled_;
    
    /**
     * @brief Initialize a single particle
     * @param particle Particle to initialize
     */
    void initParticle(Particle& particle);
    
    /**
     * @brief Find next dead particle index
     * @return Index of dead particle, or -1 if none
     */
    int findDeadParticle();
};

/**
 * @brief Preset particle effects
 */
namespace ParticlePresets {
    ParticleEmitterConfig createFire();
    ParticleEmitterConfig createSmoke();
    ParticleEmitterConfig createRain();
    ParticleEmitterConfig createSnow();
    ParticleEmitterConfig createExplosion();
    ParticleEmitterConfig createSparkle();
}

#endif // PARTICLE_H
