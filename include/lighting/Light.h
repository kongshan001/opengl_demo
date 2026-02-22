/**
 * @file Light.h
 * @brief Light system - Directional, Point, Spot lights
 */

#ifndef LIGHT_H
#define LIGHT_H

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

#include <glm/glm.hpp>
#include <memory>
#include <string>

/**
 * @brief Light type enumeration
 */
enum class LightType {
    Directional,    // Directional light (sun)
    Point,          // Point light
    Spotlight       // Spotlight
};

/**
 * @brief Base light class
 */
class Light {
public:
    Light(LightType type, const std::string& name = "");
    virtual ~Light() = default;

    // Basic properties
    void setColor(const glm::vec3& color);
    void setIntensity(float intensity);
    void setEnabled(bool enabled);

    glm::vec3 getColor() const { return color_; }
    float getIntensity() const { return intensity_; }
    bool isEnabled() const { return enabled_; }
    LightType getType() const { return type_; }
    const std::string& getName() const { return name_; }

    // Get shader data (implemented by subclasses)
    virtual void getShaderData(glm::vec3& position, glm::vec3& direction,
                               glm::vec3& color, float& intensity,
                               float& constant, float& linear, float& quadratic,
                               float& innerCutoff, float& outerCutoff) const = 0;

protected:
    LightType type_;
    std::string name_;
    glm::vec3 color_;
    float intensity_;
    bool enabled_;
};

/**
 * @brief Directional light - parallel light, simulates sun
 */
class DirectionalLight : public Light {
public:
    DirectionalLight(const std::string& name = "");
    
    void setDirection(const glm::vec3& direction);
    glm::vec3 getDirection() const { return direction_; }

    void getShaderData(glm::vec3& position, glm::vec3& direction,
                       glm::vec3& color, float& intensity,
                       float& constant, float& linear, float& quadratic,
                       float& innerCutoff, float& outerCutoff) const override;

private:
    glm::vec3 direction_;
};

/**
 * @brief Point light - with distance attenuation
 */
class PointLight : public Light {
public:
    PointLight(const std::string& name = "");

    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const { return position_; }

    // Attenuation: attenuation = 1.0 / (constant + linear*d + quadratic*d^2)
    void setAttenuation(float constant, float linear, float quadratic);
    float getConstant() const { return constant_; }
    float getLinear() const { return linear_; }
    float getQuadratic() const { return quadratic_; }

    void getShaderData(glm::vec3& position, glm::vec3& direction,
                       glm::vec3& color, float& intensity,
                       float& constant, float& linear, float& quadratic,
                       float& innerCutoff, float& outerCutoff) const override;

private:
    glm::vec3 position_;
    float constant_;    // Constant attenuation
    float linear_;      // Linear attenuation
    float quadratic_;   // Quadratic attenuation
};

/**
 * @brief Spotlight - cone light with angle limits
 */
class SpotLight : public Light {
public:
    SpotLight(const std::string& name = "");

    void setPosition(const glm::vec3& position);
    void setDirection(const glm::vec3& direction);
    void setCutoff(float innerDegrees, float outerDegrees);

    glm::vec3 getPosition() const { return position_; }
    glm::vec3 getDirection() const { return direction_; }
    float getInnerCutoff() const { return innerCutoff_; }
    float getOuterCutoff() const { return outerCutoff_; }

    void setAttenuation(float constant, float linear, float quadratic);

    void getShaderData(glm::vec3& position, glm::vec3& direction,
                       glm::vec3& color, float& intensity,
                       float& constant, float& linear, float& quadratic,
                       float& innerCutoff, float& outerCutoff) const override;

private:
    glm::vec3 position_;
    glm::vec3 direction_;
    float innerCutoff_;     // Inner cone angle (full brightness)
    float outerCutoff_;     // Outer cone angle (edge)
    float constant_;
    float linear_;
    float quadratic_;
};

#endif // LIGHT_H
