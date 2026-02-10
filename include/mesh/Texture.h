#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <iostream>

// 前向声明 - 使用extern "C"确保C链接
extern "C" {
    unsigned char* stbi_load(const char* filename, int* x, int* y, int* comp, int req_comp);
    void stbi_image_free(void* retval_from_stbi_load);
}

enum class TextureType {
    Diffuse = 0,
    Specular,
    Normal,
    Height,
    Ambient,
    Count
};

class CTexture {
public:
    unsigned int ID;
    TextureType type;
    std::string path;
    int width, height, nrChannels;
    
    // 构造函数：从文件加载
    CTexture(const std::string& filepath, TextureType texType = TextureType::Diffuse);
    
    // 构造函数：从数据创建
    CTexture(unsigned char* data, int width, int height, int channels, TextureType texType = TextureType::Diffuse);
    
    // 析构函数
    ~CTexture();
    
    // 绑定纹理
    void bind(unsigned int textureUnit = 0) const;
    
    // 解绑纹理
    static void unbind(unsigned int textureUnit = 0);
    
    // 获取纹理类型字符串
    const char* getTypeString() const;
    
    // 设置包装模式
    void setWrapMode(GLenum wrapS, GLenum wrapT);
    
    // 设置过滤模式
    void setFilterMode(GLenum minFilter, GLenum magFilter);
    
    // 生成Mipmap
    void generateMipmaps();
    
    // 获取格式信息
    GLenum getFormat() const;
    GLenum getInternalFormat() const;

private:
    // 初始化纹理
    void initialize(unsigned char* data);
    
    // 获取GL格式的辅助函数
    static GLenum getGLFormat(int channels);
    static GLenum getGLInternalFormat(int channels);
};

#endif