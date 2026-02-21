#include "shader/Shader.h"
#include <vector>

CShader::CShader(const char* vertexSource, const char* fragmentSource) {
    unsigned int vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    linkProgram(vertexShader, fragmentShader);
}

CShader::CShader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = loadShaderSource(vertexPath.c_str());
    std::string fragmentCode = loadShaderSource(fragmentPath.c_str());
    
    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();
    
    unsigned int vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    linkProgram(vertexShader, fragmentShader);
}

CShader::~CShader() {
    glDeleteProgram(ID);
}

void CShader::use() {
    glUseProgram(ID);
}

GLint CShader::getUniformLocation(const std::string& name) const {
    auto it = uniformLocationCache.find(name);
    if (it != uniformLocationCache.end()) {
        return it->second;
    }
    
    GLint location = glGetUniformLocation(ID, name.c_str());
    uniformLocationCache[name] = location;
    return location;
}

void CShader::setBool(const std::string& name, bool value) const {
    glUniform1i(getUniformLocation(name), static_cast<int>(value));
}

void CShader::setInt(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void CShader::setFloat(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void CShader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void CShader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void CShader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

unsigned int CShader::compileShader(const char* source, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        
        std::vector<char> infoLog(logLength > 0 ? logLength : 1);
        glGetShaderInfoLog(shader, logLength, NULL, infoLog.data());
        
        std::string shaderType = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
        std::string errorMsg = shaderType + " shader compilation failed: " + std::string(infoLog.data());
        
        glDeleteShader(shader);
        throw ShaderException(errorMsg);
    }
    
    return shader;
}

void CShader::linkProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    
    int success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &logLength);
        
        std::vector<char> infoLog(logLength > 0 ? logLength : 1);
        glGetProgramInfoLog(ID, logLength, NULL, infoLog.data());
        
        std::string errorMsg = "Shader program linking failed: " + std::string(infoLog.data());
        
        glDeleteProgram(ID);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        throw ShaderException(errorMsg);
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

std::string CShader::loadShaderSource(const char* filePath) {
    std::ifstream shaderFile(filePath, std::ios::in);
    if (!shaderFile.is_open()) {
        throw ShaderException("Failed to open shader file: " + std::string(filePath));
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    std::string shaderCode = shaderStream.str();
    if (shaderCode.empty()) {
        throw ShaderException("Shader file is empty: " + std::string(filePath));
    }

    return shaderCode;
}
