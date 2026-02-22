#version 330 core
// Skybox vertex shader

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aPos;
    
    // Remove translation from view matrix
    mat4 viewRot = mat4(mat3(view));
    gl_Position = projection * viewRot * vec4(aPos, 1.0);
}
