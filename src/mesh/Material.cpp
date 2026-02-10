#include "mesh/Material.h"
#include "shader/Shader.h"

CMaterial::CMaterial() 
    : diffuseColor(glm::vec3(1.0f)),
      specularColor(glm::vec3(0.5f)),
      ambientColor(glm::vec3(0.1f)),
      emissiveColor(glm::vec3(0.0f)),
      shininess(32.0f),
      specularStrength(1.0f),
      opacity(1.0f),
      refractiveIndex(1.0f),
      name("DefaultMaterial") {
}

CMaterial::CMaterial(const std::string& name)
    : diffuseColor(glm::vec3(1.0f)),
      specularColor(glm::vec3(0.5f)),
      ambientColor(glm::vec3(0.1f)),
      emissiveColor(glm::vec3(0.0f)),
      shininess(32.0f),
      specularStrength(1.0f),
      opacity(1.0f),
      refractiveIndex(1.0f),
      name(name) {
}

CMaterial::CMaterial(const CMaterial& other) {
    diffuseColor = other.diffuseColor;
    specularColor = other.specularColor;
    ambientColor = other.ambientColor;
    emissiveColor = other.emissiveColor;
    shininess = other.shininess;
    specularStrength = other.specularStrength;
    opacity = other.opacity;
    refractiveIndex = other.refractiveIndex;
    name = other.name;
    
    // 复制纹理
    textures.clear();
    for (const auto& tex : other.textures) {
        textures.push_back(tex);
    }
}

CMaterial& CMaterial::operator=(const CMaterial& other) {
    if (this != &other) {
        diffuseColor = other.diffuseColor;
        specularColor = other.specularColor;
        ambientColor = other.ambientColor;
        emissiveColor = other.emissiveColor;
        shininess = other.shininess;
        specularStrength = other.specularStrength;
        opacity = other.opacity;
        refractiveIndex = other.refractiveIndex;
        name = other.name;
        
        // 复制纹理
        textures.clear();
        for (const auto& tex : other.textures) {
            textures.push_back(tex);
        }
    }
    return *this;
}

CMaterial::~CMaterial() {
    // std::shared_ptr 会自动清理纹理
}

void CMaterial::addTexture(std::shared_ptr<CTexture> texture) {
    if (texture) {
        textures.push_back(texture);
    }
}

void CMaterial::removeTexture(size_t index) {
    if (index < textures.size()) {
        textures.erase(textures.begin() + index);
    }
}

void CMaterial::clearTextures() {
    textures.clear();
}

size_t CMaterial::getTextureCount() const {
    return textures.size();
}

std::shared_ptr<CTexture> CMaterial::getTexture(size_t index) const {
    if (index < textures.size()) {
        return textures[index];
    }
    return nullptr;
}

std::shared_ptr<CTexture> CMaterial::getTextureByType(TextureType type) const {
    for (const auto& tex : textures) {
        if (tex && tex->type == type) {
            return tex;
        }
    }
    return nullptr;
}

void CMaterial::setColors(const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& ambient) {
    diffuseColor = diffuse;
    specularColor = specular;
    ambientColor = ambient;
}

void CMaterial::setProperties(float shininess, float specularStrength, float opacity) {
    this->shininess = shininess;
    this->specularStrength = specularStrength;
    this->opacity = opacity;
}

void CMaterial::apply(CShader& shader) const {
    // 设置材质颜色
    shader.setVec3("material.diffuse", diffuseColor);
    shader.setVec3("material.specular", specularColor);
    shader.setVec3("material.ambient", ambientColor);
    shader.setVec3("material.emissive", emissiveColor);
    
    // 设置材质属性
    shader.setFloat("material.shininess", shininess);
    shader.setFloat("material.specularStrength", specularStrength);
    shader.setFloat("material.opacity", opacity);
    shader.setFloat("material.refractiveIndex", refractiveIndex);
    
    // 绑定纹理
    bindTextures(shader);
    
    // 设置是否有纹理的标志
    shader.setBool("material.hasTextures", !textures.empty());
    shader.setInt("material.textureCount", static_cast<int>(textures.size()));
}

void CMaterial::bindTextures(CShader& shader) const {
    for (size_t i = 0; i < textures.size(); ++i) {
        if (textures[i]) {
            textures[i]->bind(static_cast<unsigned int>(i));
            
            // 设置纹理名称
            std::string textureName = textures[i]->getTypeString();
            shader.setInt(textureName, static_cast<int>(i));
        }
    }
}