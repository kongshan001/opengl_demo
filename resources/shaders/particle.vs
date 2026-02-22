#version 330 core
// Particle vertex shader

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aScale;
layout (location = 3) in float aRotation;

out vec4 Color;
out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraRight;  // For billboarding
uniform vec3 cameraUp;

void main() {
    // Create billboard rotation matrix
    float cosR = cos(aRotation);
    float sinR = sin(aRotation);
    
    // Billboard: face camera
    vec3 right = cameraRight * aScale.x;
    vec3 up = cameraUp * aScale.y;
    
    // Apply rotation to billboard
    vec3 rotatedRight = right * cosR + up * sinR;
    vec3 rotatedUp = -right * sinR + up * cosR;
    
    // Quad vertices (billboard quad)
    vec3 vertexPosition = aPos + rotatedRight * (gl_VertexID == 0 || gl_VertexID == 2 ? -0.5f : 0.5f)
                               + rotatedUp * (gl_VertexID == 0 || gl_VertexID == 1 ? -0.5f : 0.5f);
    
    Color = aColor;
    TexCoords = vec2(
        gl_VertexID == 0 || gl_VertexID == 1 ? 0.0f : 1.0f,
        gl_VertexID == 0 || gl_VertexID == 2 ? 0.0f : 1.0f
    );
    
    gl_Position = projection * view * vec4(vertexPosition, 1.0);
}
