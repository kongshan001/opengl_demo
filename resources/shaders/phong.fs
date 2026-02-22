#version 330 core

// 输入
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// 输出
out vec4 FragColor;

// 材质属性
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

// 光源属性
struct LightData {
    vec3 position;      // 点光源/聚光灯位置
    vec3 direction;     // 方向光/聚光灯方向
    vec3 color;
    float intensity;
    
    // 点光源衰减
    float constant;
    float linear;
    float quadratic;
    
    // 聚光灯锥角
    float innerCutoff;
    float outerCutoff;
    
    // 类型: 0=方向光, 1=点光源, 2=聚光灯
    int type;
    int enabled;
};

#define MAX_LIGHTS 8
uniform LightData lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 ambientColor;
uniform vec3 viewPos;

// 纹理
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform bool hasDiffuseTexture;
uniform bool hasSpecularTexture;

// 计算方向光
vec3 calculateDirectionalLight(LightData light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    
    // 镜面反射 (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    // 获取材质颜色
    vec3 diffuseColor = hasDiffuseTexture ? texture(diffuseTexture, TexCoord).rgb : material.diffuse;
    vec3 specularColor = hasSpecularTexture ? texture(specularTexture, TexCoord).rgb : material.specular;
    
    // 合成
    vec3 ambient = ambientColor * material.ambient * diffuseColor;
    vec3 diffuse = light.color * light.intensity * diff * diffuseColor;
    vec3 specular = light.color * light.intensity * spec * specularColor;
    
    return ambient + diffuse + specular;
}

// 计算点光源
vec3 calculatePointLight(LightData light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    
    // 镜面反射 (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    // 获取材质颜色
    vec3 diffuseColor = hasDiffuseTexture ? texture(diffuseTexture, TexCoord).rgb : material.diffuse;
    vec3 specularColor = hasSpecularTexture ? texture(specularTexture, TexCoord).rgb : material.specular;
    
    // 合成
    vec3 ambient = ambientColor * material.ambient * diffuseColor * attenuation;
    vec3 diffuse = light.color * light.intensity * diff * diffuseColor * attenuation;
    vec3 specular = light.color * light.intensity * spec * specularColor * attenuation;
    
    return ambient + diffuse + specular;
}

// 计算聚光灯
vec3 calculateSpotLight(LightData light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 检查是否在锥形范围内
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float spotIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    
    // 如果在锥形外，不计算
    if (theta < light.outerCutoff) {
        return vec3(0.0);
    }
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    
    // 镜面反射 (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    // 获取材质颜色
    vec3 diffuseColor = hasDiffuseTexture ? texture(diffuseTexture, TexCoord).rgb : material.diffuse;
    vec3 specularColor = hasSpecularTexture ? texture(specularTexture, TexCoord).rgb : material.specular;
    
    // 合成
    vec3 ambient = ambientColor * material.ambient * diffuseColor * attenuation;
    vec3 diffuse = light.color * light.intensity * diff * diffuseColor * attenuation * spotIntensity;
    vec3 specular = light.color * light.intensity * spec * specularColor * attenuation * spotIntensity;
    
    return ambient + diffuse + specular;
}

void main() {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    
    // 遍历所有启用的光源
    for (int i = 0; i < numLights && i < MAX_LIGHTS; ++i) {
        if (lights[i].enabled == 0) continue;
        
        if (lights[i].type == 0) {
            // 方向光
            result += calculateDirectionalLight(lights[i], normal, viewDir);
        } else if (lights[i].type == 1) {
            // 点光源
            result += calculatePointLight(lights[i], normal, FragPos, viewDir);
        } else if (lights[i].type == 2) {
            // 聚光灯
            result += calculateSpotLight(lights[i], normal, FragPos, viewDir);
        }
    }
    
    // HDR 色调映射 (可选)
    // result = result / (result + vec3(1.0));
    
    FragColor = vec4(result, 1.0);
}
