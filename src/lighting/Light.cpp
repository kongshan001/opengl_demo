/**
 * @file Light.cpp
 * @brief Light system implementation
 */

#include "lighting/Light.h"
#include <cmath>

// ============== Light Base Class ==============

Light::Light(LightType type, const std::string& name)
    : type_(type)
    , name_(name)
    , color_(1.0f)
    , intensity_(1.0f)
    , enabled_(true) {
}

void Light::setColor(const glm::vec3& color) {
    color_ = color;
}

void Light::setIntensity(float intensity) {
    intensity_ = intensity;
}

void Light::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// ============== DirectionalLight ==============

DirectionalLight::DirectionalLight(const std::string& name)
    : Light(LightType::Directional, name)
    , direction_(0.0f, -1.0f, 0.0f) {
}

void DirectionalLight::setDirection(const glm::vec3& direction) {
    direction_ = glm::normalize(direction);
}

void DirectionalLight::getShaderData(glm::vec3& position, glm::vec3& direction,
                                     glm::vec3& color, float& intensity,
                                     float& constant, float& linear, float& quadratic,
                                     float& innerCutoff, float& outerCutoff) const {
    position = glm::vec3(0.0f);
    direction = direction_;
    color = color_;
    intensity = intensity_;
    constant = 1.0f;
    linear = 0.0f;
    quadratic = 0.0f;
    innerCutoff = 0.0f;
    outerCutoff = 0.0f;
}

// ============== PointLight ==============

PointLight::PointLight(const std::string& name)
    : Light(LightType::Point, name)
    , position_(0.0f)
    , constant_(1.0f)
    , linear_(0.09f)
    , quadratic_(0.032f) {
}

void PointLight::setPosition(const glm::vec3& position) {
    position_ = position;
}

void PointLight::setAttenuation(float constant, float linear, float quadratic) {
    constant_ = constant;
    linear_ = linear;
    quadratic_ = quadratic;
}

void PointLight::getShaderData(glm::vec3& position, glm::vec3& direction,
                               glm::vec3& color, float& intensity,
                               float& constant, float& linear, float& quadratic,
                               float& innerCutoff, float& outerCutoff) const {
    position = position_;
    direction = glm::vec3(0.0f);
    color = color_;
    intensity = intensity_;
    constant = constant_;
    linear = linear_;
    quadratic = quadratic_;
    innerCutoff = 0.0f;
    outerCutoff = 0.0f;
}

// ============== SpotLight ==============

SpotLight::SpotLight(const std::string& name)
    : Light(LightType::Spotlight, name)
    , position_(0.0f)
    , direction_(0.0f, -1.0f, 0.0f)
    , innerCutoff_(cosf(glm::radians(12.5f)))
    , outerCutoff_(cosf(glm::radians(17.5f)))
    , constant_(1.0f)
    , linear_(0.09f)
    , quadratic_(0.032f) {
}

void SpotLight::setPosition(const glm::vec3& position) {
    position_ = position;
}

void SpotLight::setDirection(const glm::vec3& direction) {
    direction_ = glm::normalize(direction);
}

void SpotLight::setCutoff(float innerDegrees, float outerDegrees) {
    innerCutoff_ = cosf(glm::radians(innerDegrees));
    outerCutoff_ = cosf(glm::radians(outerDegrees));
}

void SpotLight::setAttenuation(float constant, float linear, float quadratic) {
    constant_ = constant;
    linear_ = linear;
    quadratic_ = quadratic;
}

void SpotLight::getShaderData(glm::vec3& position, glm::vec3& direction,
                              glm::vec3& color, float& intensity,
                              float& constant, float& linear, float& quadratic,
                              float& innerCutoff, float& outerCutoff) const {
    position = position_;
    direction = direction_;
    color = color_;
    intensity = intensity_;
    constant = constant_;
    linear = linear_;
    quadratic = quadratic_;
    innerCutoff = innerCutoff_;
    outerCutoff = outerCutoff_;
}
