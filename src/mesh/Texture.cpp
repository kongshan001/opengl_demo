#include "mesh/Texture.h"

// stb_image implementation
#define STB_IMAGE_IMPLEMENTATION
#include "mesh/stb_image.h"

// CTexture implementation
CTexture::CTexture(const std::string& filepath, TextureType texType) 
    : type(texType), path(filepath), width(0), height(0), nrChannels(0), ID(0) {
    
    // 加载图片数据
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        initialize(data);
        stbi_image_free(data);
        std::cout << "Loaded texture: " << filepath << " (" << width << "x" << height << ")" << std::endl;
    } else {
        std::cout << "Failed to load texture: " << filepath << std::endl;
    }
}

CTexture::CTexture(unsigned char* data, int w, int h, int channels, TextureType texType)
    : type(texType), path(""), width(w), height(h), nrChannels(channels), ID(0) {
    if (data) {
        initialize(data);
        std::cout << "Created texture from data (" << width << "x" << height << ")" << std::endl;
    }
}

CTexture::~CTexture() {
    if (ID != 0) {
        glDeleteTextures(1, &ID);
    }
}

void CTexture::bind(unsigned int textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void CTexture::unbind(unsigned int textureUnit) {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

const char* CTexture::getTypeString() const {
    switch (type) {
        case TextureType::Diffuse: return "texture_diffuse";
        case TextureType::Specular: return "texture_specular";
        case TextureType::Normal: return "texture_normal";
        case TextureType::Height: return "texture_height";
        case TextureType::Ambient: return "texture_ambient";
        default: return "texture_unknown";
    }
}

void CTexture::setWrapMode(GLenum wrapS, GLenum wrapT) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

void CTexture::setFilterMode(GLenum minFilter, GLenum magFilter) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void CTexture::generateMipmaps() {
    bind();
    glGenerateMipmap(GL_TEXTURE_2D);
}

GLenum CTexture::getFormat() const {
    return getGLFormat(nrChannels);
}

GLenum CTexture::getInternalFormat() const {
    return getGLInternalFormat(nrChannels);
}

void CTexture::initialize(unsigned char* data) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    
    GLenum format = getFormat();
    GLenum internalFormat = getInternalFormat();
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    // 设置默认的纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    generateMipmaps();
}

GLenum CTexture::getGLFormat(int channels) {
    switch (channels) {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_RGB;
    }
}

GLenum CTexture::getGLInternalFormat(int channels) {
    switch (channels) {
        case 1: return GL_R8;
        case 2: return GL_RG8;
        case 3: return GL_RGB8;
        case 4: return GL_RGBA8;
        default: return GL_RGB8;
    }
}