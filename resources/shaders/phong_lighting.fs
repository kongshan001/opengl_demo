#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightData {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
    int type;
    int enabled;
};

#define MAX_LIGHTS 8

uniform Material material;
uniform LightData lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 ambientColor;
uniform vec3 viewPos;

vec3 calculateDirectionalLight(LightData light, vec3 normal, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular) {
    vec3 lightDir = normalize(-light.direction);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    vec3 ambient = ambientColor * material.ambient * matDiffuse;
    vec3 diffuse = light.color * light.intensity * diff * matDiffuse;
    vec3 specular = light.color * light.intensity * spec * matSpecular;
    
    return ambient + diffuse + specular;
}

vec3 calculatePointLight(LightData light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 matDiffuse, vec3 matSpecular) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    vec3 ambient = ambientColor * material.ambient * matDiffuse * attenuation;
    vec3 diffuse = light.color * light.intensity * diff * matDiffuse * attenuation;
    vec3 specular = light.color * light.intensity * spec * matSpecular * attenuation;
    
    return ambient + diffuse + specular;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 matDiffuse = material.diffuse;
    vec3 matSpecular = material.specular;
    
    vec3 result = vec3(0.0);
    
    for (int i = 0; i < numLights && i < MAX_LIGHTS; ++i) {
        if (lights[i].enabled == 0) continue;
        
        if (lights[i].type == 0) {
            result += calculateDirectionalLight(lights[i], norm, viewDir, matDiffuse, matSpecular);
        } else if (lights[i].type == 1) {
            result += calculatePointLight(lights[i], norm, FragPos, viewDir, matDiffuse, matSpecular);
        }
    }
    
    FragColor = vec4(result, 1.0);
}
