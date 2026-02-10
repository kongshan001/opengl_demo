#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class CShader {
public:
    unsigned int ID;

    CShader(const char* vertexSource, const char* fragmentSource);
    CShader(const std::string& vertexPath, const std::string& fragmentPath);
    ~CShader();

    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

private:
    unsigned int compileShader(const char* source, unsigned int type);
    void linkProgram(unsigned int vertexShader, unsigned int fragmentShader);
    std::string loadShaderSource(const char* filePath);
};

#endif
