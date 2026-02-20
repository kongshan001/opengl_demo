#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "mesh/Texture.h"

class CShader;

class CMaterial {
public:
    // 基础属性
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    glm::vec3 ambientColor;
    glm::vec3 emissiveColor;
    
    // 光照参数
    float shininess;
    float specularStrength;
    float opacity;
    float refractiveIndex;
    
    // 纹理
    std::vector<std::shared_ptr<CTexture>> textures;
    
    // 构造函数
    CMaterial();
    CMaterial(const std::string& name);
    
    // 拷贝/赋值
    CMaterial(const CMaterial& other);
    CMaterial& operator=(const CMaterial& other);
    
    // 析构函数
    ~CMaterial();
    
    // Shader管理
    void setShader(std::shared_ptr<CShader> shader);
    std::shared_ptr<CShader> getShader() const { return shader; }
    bool hasShader() const { return shader != nullptr; }
    
    // 纹理管理
    void addTexture(std::shared_ptr<CTexture> texture);
    void removeTexture(size_t index);
    void clearTextures();
    size_t getTextureCount() const;
    std::shared_ptr<CTexture> getTexture(size_t index) const;
    std::shared_ptr<CTexture> getTextureByType(TextureType type) const;
    
    // 材质属性设置
    void setColors(const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& ambient);
    void setProperties(float shininess, float specularStrength, float opacity = 1.0f);
    
    // 应用材质（使用内置Shader）
    void apply() const;
    
    // 应用材质到指定着色器（兼容旧接口）
    void applyToShader(CShader& shader) const;
    
    // 材质信息
    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }
    
    // 是否有纹理
    bool hasTextures() const { return !textures.empty(); }
    
    // 是否为透明材质
    bool isTransparent() const { return opacity < 1.0f; }

private:
    std::string name;
    std::shared_ptr<CShader> shader;
    
    // 绑定纹理到着色器
    void bindTextures(CShader& shader) const;
};

#endif