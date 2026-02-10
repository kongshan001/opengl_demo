#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

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
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    if (material.hasTextures) {
        // 简单的漫反射光照
        vec3 ambient = material.ambient * material.diffuse;
        
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * material.diffuse;
        
        vec3 result = ambient + diffuse;
        FragColor = vec4(result, material.opacity);
    } else {
        // 简单的颜色输出
        FragColor = vec4(material.diffuse, material.opacity);
    }
}