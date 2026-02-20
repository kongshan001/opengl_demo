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
      name("DefaultMaterial"),
      shader(nullptr) {
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
      name(name),
      shader(nullptr) {
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
    shader = other.shader;
    
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
        shader = other.shader;
        
        // 复制纹理
        textures.clear();
        for (const auto& tex : other.textures) {
            textures.push_back(tex);
        }
    }
    return *this;
}

CMaterial::~CMaterial() {
}

void CMaterial::setShader(std::shared_ptr<CShader> shader) {
    this->shader = shader;
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

void CMaterial::apply() const {
    if (!shader) return;
    
    shader->use();
    applyToShader(*shader);
}

void CMaterial::applyToShader(CShader& targetShader) const {
    // 设置材质颜色
    targetShader.setVec3("material.diffuse", diffuseColor);
    targetShader.setVec3("material.specular", specularColor);
    targetShader.setVec3("material.ambient", ambientColor);
    targetShader.setVec3("material.emissive", emissiveColor);
    
    // 设置材质属性
    targetShader.setFloat("material.shininess", shininess);
    targetShader.setFloat("material.specularStrength", specularStrength);
    targetShader.setFloat("material.opacity", opacity);
    targetShader.setFloat("material.refractiveIndex", refractiveIndex);
    
    // 绑定纹理
    bindTextures(targetShader);
    
    // 设置是否有纹理的标志
    targetShader.setBool("material.hasTextures", !textures.empty());
    targetShader.setInt("material.textureCount", static_cast<int>(textures.size()));
}

void CMaterial::bindTextures(CShader& targetShader) const {
    for (size_t i = 0; i < textures.size(); ++i) {
        if (textures[i]) {
            textures[i]->bind(static_cast<unsigned int>(i));
            
            // 设置纹理名称
            std::string textureName = textures[i]->getTypeString();
            targetShader.setInt(textureName, static_cast<int>(i));
        }
    }
}