#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform bool hasDiffuseTexture;
uniform vec3 materialDiffuse;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main() {
    vec3 color;
    if (hasDiffuseTexture) {
        color = texture(diffuseTexture, TexCoord).rgb;
    } else {
        color = materialDiffuse;
    }
    
    // 方向光（从 lightPos 方向照射）
    vec3 lightDir = normalize(lightPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // 基础亮度 + 漫反射
    float brightness = 0.3 + 0.7 * diff;
    
    // 应用光照颜色
    vec3 result = color * brightness * lightColor;
    
    FragColor = vec4(result, 1.0);
}
