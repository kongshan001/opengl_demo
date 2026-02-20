# 材质和纹理

本教程将展示如何在OpenGL Demo中使用CMaterial和纹理系统，创建更丰富的视觉效果。

## 🎯 学习目标
- 理解材质的基本概念和属性
- 学会加载和使用纹理
- 掌握多纹理技术
- 创建具有真实感的3D对象

## 📋 基础概念

### 材质组成
现代材质通常包含以下属性：

1. **漫反射(Diffuse)**：对象的主要颜色
2. **镜面反射(Specular)**：高光反射
3. **环境光(Ambient)**：环境光照贡献的颜色
4. **自发光(Emissive)**：对象自身发光的颜色
5. **粗糙度(Shininess)**：高光反射的锐利程度

### 纹理作用
- 提供表面细节和颜色变化
- 控制光照反射效果
- 增强真实感

### PBR材质（物理基础渲染）
现代引擎使用基于物理的材质模型：
- **金属度(Metallic)**：表面金属特性，影响反射率
- **粗糙度(Roughness)**：表面粗糙程度，影响漫反射和镜面反射的分布

## 🎨 CMaterial类概览

### 基本属性
```cpp
class CMaterial {
    // 颜色属性
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    glm::vec3 ambientColor;
    glm::vec3 emissiveColor;
    
    // 光照参数
    float shininess;      // 高光反射锐利度 (通常32-256)
    float specularStrength; // 镜面反射强度 (通常0.0-1.0)
    float opacity;       // 透明度 (0.0-1.0)
    float refractiveIndex;  // 折射率 (通常1.0)
    
    // 纹理系统
    std::vector<std::shared_ptr<CTexture>> textures;
    
    // 方便方法
    void setColors(const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& ambient);
    void setProperties(float shininess, float specularStrength, float opacity = 1.0f);
};
```

## 🚀 开始使用

### 1. 创建基础材质
```cpp
#include "mesh/Material.h"

// 创建简单的漫反射材质
auto material = std::make_shared<CMaterial>("RedMaterial");
material->setColors(
    glm::vec3(0.8f, 0.2f, 0.1f),  // 红色
    glm::vec3(1.0f, 1.0f, 1.0f),  // 镜面反射
    glm::vec3(0.1f, 0.1f, 0.1f)      // 环境光
);
```

### 2. 创建金属材质
```cpp
auto metalMaterial = std::make_shared<CMaterial>("MetalMaterial");
metalMaterial->setColors(
    glm::vec3(0.7f, 0.6f, 0.8f),   // 暗灰金属
    glm::vec3(1.0f, 1.0f, 1.0f),  // 强镜面反射
    glm::vec3(0.2f, 0.2f, 0.2f)   // 环境光
);
metalMaterial->setProperties(64.0f, 0.8f);  // 高镜面锐利度
```

### 3. 创建发光材质
```cpp
auto emissiveMaterial = std::make_shared<CMaterial>("GlowMaterial");
emissiveMaterial->setColors(
    glm::vec3(0.0f, 0.0f, 0.0f),  // 黑色基础
    glm::vec3(0.0f, 0.0f, 1.0f)    // 自发光
emissiveMaterial->setProperties(1.0f, 1.0f);  // 禁用镜面反射
```

## 🎯 着色器集成

### 在着色器中访问材质
```glsl
#version 330 core

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 emissive;
    float shininess;
    float specularStrength;
    float opacity;
    bool hasTextures;
    int textureCount;
};

uniform Material material;
uniform sampler2D texture_diffuse[16];
uniform sampler2D texture_normal[16];

void main() {
    // 获取漫反射
    vec3 diffuse = texture(material.texture_diffuse[0], TexCoord).rgb * material.diffuse;
    
    // 获取法线和镜面反射
    vec3 normal = texture(material.texture_normal[0], TexCoord).rgb * 2.0 - 1.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    float diff = max(dot(normal, viewDir), 0.0);
    vec3 specular = pow(max(dot(viewDir, reflect(viewDir, normal)), material.shininess);
    
    // 计算最终颜色
    vec3 result = diffuse + specular + material.ambient + material.emissive;
    FragColor = vec4(result, material.opacity);
}
```

## � 加载纹理

### 1. 基础纹理加载
```cpp
#include "mesh/Texture.h"

// 从文件创建纹理
auto texture = std::make_shared<CTexture>("resources/textures/wall.jpg", TextureType::Diffuse);

// 创建材质并添加纹理
auto material = std::make_shared<CMaterial>("WallMaterial");
material->addTexture(texture);
```

### 2. 纹理参数设置
```cpp
// 设置纹理包装模式（平铺）
texture->setWrapMode(GL_REPEAT, GL_REPEAT);

// 设置过滤模式（线性插值）
texture->setFilterMode(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);

// 自动生成mipmap
texture->generateMipmaps();
```

### 3. 多纹理支持
```cpp
// 为材质添加多个纹理
auto material = std::make_shared<CMaterial>("MultiTextureMaterial");

// 添加不同类型的纹理
material->addTexture(diffuseTexture);  // 漫反射纹理
material->addTexture(normalTexture);   // 法线贴图
material->addTexture(specularTexture); // 镜面贴图
material->addTexture(emissiveTexture); // 自发光纹理

// 在着色器中通过类型访问
shader.setInt("texture_diffuse", 0);
shader.setInt("texture_normal", 1);
shader.setInt("texture_specular", 2);
```

## 🔄 实际应用示例

### 1. 纹理木材质
```cpp
// 创建木纹材质
auto woodMaterial = std::make_shared<CMaterial>("WoodMaterial");
woodMaterial->setColors(
    glm::vec3(0.6f, 0.4f, 0.2f),  // 棕木纹理色
    glm::vec3(0.04f, 0.02f, 0.01f),  // 低镜面反射
    glm::vec3(0.1f, 0.1f, 0.1f)   // 环境光
);
woodMaterial->setProperties(16.0f, 0.3f);  // 低镜面锐利度

// 加载木纹纹理
auto woodTexture = std::make_shared<CTexture>("resources/textures/wood.jpg", TextureType::Diffuse);
woodMaterial->addTexture(woodTexture);
```

### 2. 金属球体
```cpp
auto metalMaterial = std::make_shared<CMaterial>("MetalSphere");
metalMaterial->setColors(
    glm::vec3(0.7f, 0.7f, 0.9f),  // 银金色泽
    glm::vec3(1.0f, 1.0f, 1.0f)  // 强镜面反射
    glm::vec3(0.2f, 0.2f, 0.2f)  // 环境光
);

// 创建金属球体
auto sphere = MeshUtils::createSphere(1.0f, 32);
sphere.setMaterial(metalMaterial);

// 渲染
sphere.draw();
```

### 3. 发光材质
```cpp
auto glowMaterial = std::make_shared<CMaterial>("GlowEffect");
glowMaterial->setColors(
    glm::vec3(0.0f, 0.8f, 1.0f), // 发光颜色
    glm::vec3(0.0f, 0.0f, 1.0f)  // 边缘发光
);
glowMaterial->setProperties(1.0f, 1.0f);  // 禁用镜面反射

// 创建发光球体
auto glowSphere = MeshUtils::createSphere(1.0f, 32);
glowSphere.setMaterial(glowMaterial);

// 添加发光纹理
auto glowTexture = std::make_shared<CTexture>("resources/textures/glow.jpg", TextureType::Emissive);
glowMaterial->addTexture(glowTexture);
```

## 🔧 进阶主题

### PBR材质
```cpp
// 创建PBR材质
auto pbrMaterial = std::make_shared<CMaterial>("PBR_Material");
pbrMaterial->setColors(
    glm::vec3(0.5f, 0.5f, 0.5f),  // 中性色
    glm::vec3(0.0f, 0.0f, 1.0f)  // 无环境光
);

// 设置PBR属性
pbrMaterial->setProperties(32.0f, 0.5f);
pbrMaterial->setMetallic(0.8f); // 80%金属度

// PBR着色器需要额外的属性
// ...
```

### 纹理动画
```cpp
// 时间变化材质（模拟材料磨损）
auto createTimeVaryingMaterial = [&](const std::string& name) {
    auto material = std::make_shared<CMaterial>(name);
    
    material->setColors(
        glm::vec3(0.8f, 0.6f, 0.4f),  // 初始颜色
        glm::vec3(0.2f, 0.1f, 0.1f)   // 磀久变化
        glm::vec3(0.1f, 0.0f, 0.1f)   // 最终颜色
    );
    
    // 返回一个可更新的材质对象
    return material;
};
```

## 🛠️ 常见问题

### Q: 如何切换不同材质？
```cpp
// 方法1：材质对象切换
std::vector<std::shared_ptr<CMaterial>> materials = {
    woodMaterial, metalMaterial, glassMaterial
};

// 在运行时切换
currentMaterial = materials[currentMaterialIndex];

// 方法2：属性动态调整
material->setColors(newDiffuse, newSpecular, newAmbient);
material->setProperties(newShininess, newSpecularStrength);
```

### Q: 如何混合多种纹理？
```cpp
// 设置混合权重
shader.setFloat("diffuseWeight", 0.8f);
shader.setFloat("normalWeight", 0.2f);
shader.setFloat("specularWeight", 0.5f);

// 在着色器中：
// 混合计算
vec3 mixedColor = 
    diffuseColor * diffuseWeight +
    normalColor * normalWeight +
    specularColor * specularWeight;
```

### Q: 纹理透明度
```cpp
// 设置透明度
material->setOpacity(0.5f); // 半透明
shader.setFloat("material.opacity", 0.5f);
```

### Q: 如何优化纹理内存？
```cpp
// 使用纹理压缩格式
auto compressedTexture = std::make_shared<CTexture>("texture.jpg", TextureType::Diffuse);

// 设置压缩选项（需要特定的图像格式）
// compressedTexture->setCompressionType(GL_COMPRESS_DXT3);  // DirectXT3压缩

// 设置各向异性过滤
texture->setAnisotropicFiltering(true);
```

现在你掌握了完整的材质和纹理系统！
选择一个材质主题开始实践，或混合多种材质创建更复杂的视觉效果。