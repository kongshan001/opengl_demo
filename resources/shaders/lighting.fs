#version 330 core
// Lighting fragment shader - supports up to 8 dynamic lights with shadows

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;  // For shadow calculation

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Light data structure (packed for array)
struct LightData {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    
    float constant;
    float linear;
    float quadratic;
    
    float innerCutoff;
    float outerCutoff;
    
    int type;  // 0=directional, 1=point, 2=spotlight
};

uniform Material material;
uniform LightData lights[8];
uniform int numLights;
uniform vec3 viewPos;
uniform vec3 ambientColor;

// Shadow mapping
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;
uniform int shadowsEnabled;
uniform float shadowBiasMin;
uniform float shadowBiasMax;
uniform int pcfEnabled;

// Texture
uniform sampler2D diffuseTexture;
uniform int hasDiffuseTexture;

vec3 calculateDirectionalLight(LightData light, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(LightData light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateSpotLight(LightData light, vec3 normal, vec3 fragPos, vec3 viewDir);
float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main() {
    // Get base color from texture or material
    vec3 baseColor;
    if (hasDiffuseTexture == 1) {
        baseColor = texture(diffuseTexture, TexCoords).rgb;
    } else {
        baseColor = material.diffuse;
    }
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Start with ambient
    vec3 result = ambientColor * baseColor * material.ambient;
    
    // Add contribution from each light
    for (int i = 0; i < numLights && i < 8; i++) {
        vec3 lightContrib;
        vec3 lightDir;
        
        if (lights[i].type == 0) {
            // Directional light
            lightContrib = calculateDirectionalLight(lights[i], norm, viewDir);
            lightDir = normalize(-lights[i].direction);
        } else if (lights[i].type == 1) {
            // Point light
            lightContrib = calculatePointLight(lights[i], norm, FragPos, viewDir);
            lightDir = normalize(lights[i].position - FragPos);
        } else if (lights[i].type == 2) {
            // Spotlight
            lightContrib = calculateSpotLight(lights[i], norm, FragPos, viewDir);
            lightDir = normalize(lights[i].position - FragPos);
        } else {
            continue;
        }
        
        // Calculate shadow for directional lights only
        float shadow = 0.0;
        if (shadowsEnabled == 1 && lights[i].type == 0) {
            shadow = calculateShadow(FragPosLightSpace, norm, lightDir);
        }
        
        result += lightContrib * baseColor * (1.0 - shadow);
    }
    
    FragColor = vec4(result, 1.0);
}

vec3 calculateDirectionalLight(LightData light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    // Combine
    vec3 diffuse = diff * light.color * light.intensity;
    vec3 specular = spec * light.color * light.intensity * material.specular;
    
    return diffuse + specular;
}

vec3 calculatePointLight(LightData light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    // Combine
    vec3 diffuse = diff * light.color * light.intensity * attenuation;
    vec3 specular = spec * light.color * light.intensity * material.specular * attenuation;
    
    return diffuse + specular;
}

vec3 calculateSpotLight(LightData light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    // Spotlight cone
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    
    // Combine
    vec3 diffuse = diff * light.color * light.intensity * attenuation * intensity;
    vec3 specular = spec * light.color * light.intensity * material.specular * attenuation * intensity;
    
    return diffuse + specular;
}

float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // Check whether current frag pos is in shadow
    // Apply bias based on surface angle
    float bias = max(shadowBiasMax * (1.0 - dot(normal, lightDir)), shadowBiasMin);
    
    // PCF (Percentage Closer Filtering) for soft shadows
    float shadow = 0.0;
    
    if (pcfEnabled == 1) {
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        int kernelSize = 2;
        int samples = 0;
        
        for (int x = -kernelSize; x <= kernelSize; ++x) {
            for (int y = -kernelSize; y <= kernelSize; ++y) {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                samples++;
            }
        }
        shadow /= float(samples);
    } else {
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
    
    // Keep the shadow at 0.0 when outside the far plane region
    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }
    
    return shadow;
}
