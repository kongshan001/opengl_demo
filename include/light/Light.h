/**
 * @file Light.h
 * @brief Enhanced Phong lighting system - supports ambient, diffuse, specular
 */

#ifndef LIGHT_PHONG_H
#define LIGHT_PHONG_H

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
#include <string>
#include <array>

enum class LightTypePhong {
    Directional,
    Point
};

struct LightShaderData {
    glm::vec3 position;
    float _pad0;
    glm::vec3 direction;
    float _pad1;
    glm::vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
    int type;
    int enabled;
};

class PhongLight {
public:
    PhongLight(LightTypePhong type, const std::string& name = "");
    virtual ~PhongLight() = default;

    void setColor(const glm::vec3& color) { color_ = color; }
    void setIntensity(float intensity) { intensity_ = intensity; }
    void setEnabled(bool enabled) { enabled_ = enabled; }

    glm::vec3 getColor() const { return color_; }
    float getIntensity() const { return intensity_; }
    bool isEnabled() const { return enabled_; }
    LightTypePhong getType() const { return type_; }
    const std::string& getName() const { return name_; }

    virtual LightShaderData getShaderData() const = 0;

protected:
    LightTypePhong type_;
    std::string name_;
    glm::vec3 color_;
    float intensity_;
    bool enabled_;
};

class DirectionalLightPhong : public PhongLight {
public:
    explicit DirectionalLightPhong(const std::string& name = "");

    void setDirection(const glm::vec3& dir);
    glm::vec3 getDirection() const { return direction_; }

    LightShaderData getShaderData() const override;

private:
    glm::vec3 direction_;
};

class PointLightPhong : public PhongLight {
public:
    explicit PointLightPhong(const std::string& name = "");

    void setPosition(const glm::vec3& pos);
    glm::vec3 getPosition() const { return position_; }

    void setAttenuation(float constant, float linear, float quadratic);
    float getConstant() const { return constant_; }
    float getLinear() const { return linear_; }
    float getQuadratic() const { return quadratic_; }

    LightShaderData getShaderData() const override;

private:
    glm::vec3 position_;
    float constant_;
    float linear_;
    float quadratic_;
};

struct PhongMaterial {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    PhongMaterial()
        : ambient(0.1f)
        , diffuse(0.8f)
        , specular(1.0f)
        , shininess(32.0f) {}

    PhongMaterial(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, float sh)
        : ambient(a), diffuse(d), specular(s), shininess(sh) {}
};

#endif
