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

// 计算单个点光源的贡献
vec3 calculatePointLight(PointLight light, vec3 baseColor, vec3 norm, vec3 fragPos, vec3 viewDir) {
    // 距离衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    // 环境光
    vec3 ambient = light.ambientStrength * light.color * baseColor;
    
    // 漫反射
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuseStrength * diff * light.color * baseColor;
    
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specularStrength * spec * light.color * material.specular;
    
    // 应用衰减
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

void main() {
    // 获取基础颜色
    vec3 baseColor;
    if (hasDiffuseTexture) {
        baseColor = texture(diffuseTexture, TexCoord).rgb;
    } else {
        baseColor = material.diffuse;
    }
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // 累加所有光源的贡献
    vec3 result = vec3(0.0);
    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        result += calculatePointLight(lights[i], baseColor, norm, FragPos, viewDir);
    }
    
    FragColor = vec4(result, 1.0);
}
