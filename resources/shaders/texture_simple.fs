#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform bool hasDiffuseTexture;
uniform vec3 materialDiffuse;

void main() {
    vec3 color;
    if (hasDiffuseTexture) {
        color = texture(diffuseTexture, TexCoord).rgb;
    } else {
        color = materialDiffuse;
    }
    
    // 简单的光照：只用法线和方向光
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.8));
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // 基础亮度 + 漫反射
    float brightness = 0.4 + 0.6 * diff;
    
    FragColor = vec4(color * brightness, 1.0);
}
