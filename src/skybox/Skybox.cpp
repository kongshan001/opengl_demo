/**
 * @file Skybox.cpp
 * @brief Skybox rendering implementation
 */

#include "skybox/Skybox.h"
#include "shader/Shader.h"
#include <iostream>
#include <stb_image.h>

Skybox::Skybox()
    : vao_(0)
    , vbo_(0)
    , cubemapTexture_(0)
    , enabled_(true)
    , yaw_(0.0f)
    , pitch_(0.0f) {
}

Skybox::~Skybox() {
    if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
    if (cubemapTexture_ != 0) glDeleteTextures(1, &cubemapTexture_);
}

bool Skybox::initialize() {
    std::shared_ptr<CShader> shader = std::make_shared<CShader>(
        std::string("resources/shaders/skybox.vs"),
        std::string("resources/shaders/skybox.fs")
    );
    
    if (!createBuffers()) {
        return false;
    }
    
    std::cout << "Skybox initialized" << std::endl;
    return true;
}

bool Skybox::createBuffers() {
    // Skybox cube vertices
    float vertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f
    };
    
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glBindVertexArray(0);
    return true;
}

bool Skybox::loadCubemap(const std::vector<std::string>& faces) {
    if (faces.size() != 6) {
        std::cerr << "Cubemap requires exactly 6 faces" << std::endl;
        return false;
    }
    
    glGenTextures(1, &cubemapTexture_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture_);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // Load each face
    for (int i = 0; i < 6; ++i) {
        int width, height, channels;
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
        
        if (!data) {
            std::cerr << "Failed to load skybox face: " << faces[i] << std::endl;
            continue;
        }
        
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        
        glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return true;
}

void Skybox::setRotation(float yaw, float pitch) {
    yaw_ = yaw;
    pitch_ = pitch;
}

void Skybox::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!enabled_ || cubemapTexture_ == 0) return;
    
    // Load shader
    std::shared_ptr<CShader> shader = std::make_shared<CShader>(
        std::string("resources/shaders/skybox.vs"),
        std::string("resources/shaders/skybox.fs")
    );
    shader->use();
    
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    
    // Bind skybox cubemap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture_);
    shader->setInt("skybox", 0);
    
    // Disable depth writing (render at far plane)
    glDepthMask(GL_FALSE);
    
    // Render skybox cube
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    // Restore depth writing
    glDepthMask(GL_TRUE);
}

// ==================== SkyboxPresets ====================

namespace SkyboxPresets {

std::vector<std::string> createDay() {
    // Daytime skybox faces
    return {
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/back.jpg",
        "resources/textures/skybox/front.jpg"
    };
}

std::vector<std::string> createNight() {
    // Night skybox faces (placeholder paths)
    return {
        "resources/textures/skybox/right_night.jpg",
        "resources/textures/skybox/left_night.jpg",
        "resources/textures/skybox/top_night.jpg",
        "resources/textures/skybox/bottom_night.jpg",
        "resources/textures/skybox/back_night.jpg",
        "resources/textures/skybox/front_night.jpg"
    };
}

std::vector<std::string> createSunset() {
    // Sunset skybox faces (placeholder paths)
    return {
        "resources/textures/skybox/right_sunset.jpg",
        "resources/textures/skybox/left_sunset.jpg",
        "resources/textures/skybox/top_sunset.jpg",
        "resources/textures/skybox/bottom_sunset.jpg",
        "resources/textures/skybox/back_sunset.jpg",
        "resources/textures/skybox/front_sunset.jpg"
    };
}

std::vector<std::string> createCloudy() {
    // Cloudy skybox faces (placeholder paths)
    return {
        "resources/textures/skybox/right_cloudy.jpg",
        "resources/textures/skybox/left_cloudy.jpg",
        "resources/textures/skybox/top_cloudy.jpg",
        "resources/textures/skybox/bottom_cloudy.jpg",
        "resources/textures/skybox/back_cloudy.jpg",
        "resources/textures/skybox/front_cloudy.jpg"
    };
}

} // namespace SkyboxPresets
