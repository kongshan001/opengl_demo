#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// 材质结构
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// 光源结构
struct Light {
    vec3 position;
    vec3 color;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

// 纹理
uniform sampler2D diffuseTexture;
uniform bool hasDiffuseTexture;

void main() {
    // 获取基础颜色（纹理或材质颜色）
    vec3 baseColor;
    if (hasDiffuseTexture) {
        baseColor = texture(diffuseTexture, TexCoord).rgb;
    } else {
        baseColor = material.diffuse;
    }
    
    // 1. 环境光 (Ambient)
    vec3 ambient = light.ambientStrength * light.color * baseColor;
    
    // 2. 漫反射 (Diffuse)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuseStrength * diff * light.color * baseColor;
    
    // 3. 镜面反射 (Specular)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specularStrength * spec * light.color * material.specular;
    
    // 合并光照
    vec3 result = ambient + diffuse + specular;
    
    FragColor = vec4(result, 1.0);
}
