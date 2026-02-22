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

// 点光源结构
struct PointLight {
    vec3 position;
    vec3 color;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    
    // 衰减参数
    float constant;
    float linear;
    float quadratic;
};

#define MAX_LIGHTS 4

uniform Material material;
uniform PointLight lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;

// 纹理
uniform sampler2D diffuseTexture;
uniform bool hasDiffuseTexture;

uniform sampler2D specularTexture;
uniform bool hasSpecularTexture;

uniform sampler2D normalTexture;
uniform bool hasNormalTexture;

// 计算单个点光源的贡献
vec3 calculatePointLight(PointLight light, vec3 baseColor, vec3 normal, vec3 fragPos, vec3 viewDir) {
    // 距离衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    // 环境光
    vec3 ambient = light.ambientStrength * light.color * baseColor;
    
    // 漫反射
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuseStrength * diff * light.color * baseColor;
    
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specularStrength * spec * light.color * material.specular;
    
    // 应用衰减
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

void main() {
    // 获取基础漫反射颜色
    vec3 diffuseColor;
    if (hasDiffuseTexture) {
        diffuseColor = texture(diffuseTexture, TexCoord).rgb;
    } else {
        diffuseColor = material.diffuse;
    }
    
    // 获取镜面反射强度
    float specularStrength;
    if (hasSpecularTexture) {
        specularStrength = texture(specularTexture, TexCoord).r;
    } else {
        specularStrength = 1.0f;
    }
    
    // 获取法线（如果有法线贴图）
    vec3 normal = normalize(Normal);
    if (hasNormalTexture) {
        // 从法线贴图读取法线（从切线空间转换到世界空间）
        vec3 tangentNormal = texture(normalTexture, TexCoord).rgb * 2.0 - 1.0;
        // TODO: 需要计算 TBN 矩阵进行变换
        // 目前直接使用顶点法线
        normal = normalize(Normal);
    }
    
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // 更新材质镜面反射颜色（考虑纹理）
    vec3 finalSpecular = material.specular * specularStrength;
    
    // 累加所有光源的贡献
    vec3 result = vec3(0.0);
    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        PointLight adjustedLight = lights[i];
        adjustedLight.specularStrength *= specularStrength;
        result += calculatePointLight(adjustedLight, diffuseColor, normal, FragPos, viewDir);
    }
    
    FragColor = vec4(result, 1.0);
}
