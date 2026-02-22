#version 330 core
// Particle fragment shader

out vec4 FragColor;

in vec4 Color;
in vec2 TexCoords;

uniform sampler2D particleTexture;
uniform int hasTexture;

void main() {
    if (hasTexture == 1) {
        FragColor = texture(particleTexture, TexCoords) * Color;
    } else {
        // Default: soft circle
        vec2 center = TexCoords - vec2(0.5);
        float dist = length(center);
        float alpha = 1.0 - smoothstep(0.3, 0.5, dist);
        FragColor = vec4(Color.rgb, Color.a * alpha);
    }
}
