/**
 * @file Light.cpp
 * @brief Phong lighting system implementation
 */

#include "light/Light.h"
#include <cmath>

PhongLight::PhongLight(LightTypePhong type, const std::string& name)
    : type_(type)
    , name_(name)
    , color_(1.0f)
    , intensity_(1.0f)
    , enabled_(true) {
}

DirectionalLightPhong::DirectionalLightPhong(const std::string& name)
    : PhongLight(LightTypePhong::Directional, name)
    , direction_(0.0f, -1.0f, 0.0f) {
}

void DirectionalLightPhong::setDirection(const glm::vec3& dir) {
    direction_ = glm::normalize(dir);
}

LightShaderData DirectionalLightPhong::getShaderData() const {
    LightShaderData data;
    data.position = glm::vec3(0.0f);
    data.direction = direction_;
    data.color = color_;
    data.intensity = intensity_;
    data.constant = 1.0f;
    data.linear = 0.0f;
    data.quadratic = 0.0f;
    data.type = 0;
    data.enabled = enabled_ ? 1 : 0;
    return data;
}

PointLightPhong::PointLightPhong(const std::string& name)
    : PhongLight(LightTypePhong::Point, name)
    , position_(0.0f)
    , constant_(1.0f)
    , linear_(0.09f)
    , quadratic_(0.032f) {
}

void PointLightPhong::setPosition(const glm::vec3& pos) {
    position_ = pos;
}

void PointLightPhong::setAttenuation(float constant, float linear, float quadratic) {
    constant_ = constant;
    linear_ = linear;
    quadratic_ = quadratic;
}

LightShaderData PointLightPhong::getShaderData() const {
    LightShaderData data;
    data.position = position_;
    data.direction = glm::vec3(0.0f);
    data.color = color_;
    data.intensity = intensity_;
    data.constant = constant_;
    data.linear = linear_;
    data.quadratic = quadratic_;
    data.type = 1;
    data.enabled = enabled_ ? 1 : 0;
    return data;
}
