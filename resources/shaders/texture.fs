#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float shininess;
    float specularStrength;
    float opacity;
};

uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform bool hasDiffuseTexture;
uniform bool hasSpecularTexture;

void main() {
    // 基础颜色（纹理或材质颜色）
    vec3 color;
    if (hasDiffuseTexture) {
        color = texture(diffuseTexture, TexCoord).rgb;
    } else {
        color = material.diffuse;
    }

    // 环境光
    vec3 ambient = material.ambient * color;

    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // 高光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 specularColor;
    if (hasSpecularTexture) {
        specularColor = texture(specularTexture, TexCoord).rgb;
    } else {
        specularColor = material.specular;
    }
    vec3 specular = material.specularStrength * spec * specularColor;

    // 最终颜色
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, material.opacity);
}
