#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief Exception class for shader-related errors.
 * 
 * This exception is thrown when shader compilation, linking,
 * or file loading operations fail.
 */
class ShaderException : public std::exception {
public:
    /**
     * @brief Constructs a ShaderException with the given message.
     * @param message The error message describing the exception.
     */
    explicit ShaderException(const std::string& message) : msg(message) {}
    
    /**
     * @brief Returns the error message.
     * @return C-style string containing the error message.
     */
    const char* what() const noexcept override { return msg.c_str(); }
    
private:
    std::string msg;
};

/**
 * @brief OpenGL shader program wrapper class.
 * 
 * This class provides a high-level interface for creating, compiling,
 * and using OpenGL shader programs. It supports vertex and fragment shaders,
 * and provides convenient methods for setting uniform values.
 * 
 * @note This class uses RAII for resource management. The OpenGL program
 *       is automatically deleted when the object is destroyed.
 * 
 * Example usage:
 * @code
 * CShader shader("vertex.glsl", "fragment.glsl");
 * shader.use();
 * shader.setMat4("model", modelMatrix);
 * @endcode
 */
class CShader {
public:
    /**
     * @brief The OpenGL program ID.
     * 
     * This public member provides direct access to the OpenGL shader program
     * identifier for advanced use cases.
     */
    unsigned int ID;

    /**
     * @brief Constructs a shader program from source strings.
     * 
     * Creates and links a shader program using the provided vertex and
     * fragment shader source code strings.
     * 
     * @param vertexSource The vertex shader source code.
     * @param fragmentSource The fragment shader source code.
     * @throws ShaderException If shader compilation or program linking fails.
     */
    CShader(const char* vertexSource, const char* fragmentSource);
    
    /**
     * @brief Constructs a shader program from shader files.
     * 
     * Loads shader source code from the specified files, compiles them,
     * and links them into a shader program.
     * 
     * @param vertexPath Path to the vertex shader file.
     * @param fragmentPath Path to the fragment shader file.
     * @throws ShaderException If file loading, compilation, or linking fails.
     */
    CShader(const std::string& vertexPath, const std::string& fragmentPath);
    
    /**
     * @brief Destructor.
     * 
     * Automatically deletes the OpenGL shader program to prevent resource leaks.
     */
    ~CShader();

    /**
     * @brief Activates this shader program for rendering.
     * 
     * Call this method before rendering objects that should use this shader.
     */
    void use();
    
    /**
     * @brief Sets a boolean uniform value.
     * @param name The name of the uniform variable in the shader.
     * @param value The boolean value to set.
     */
    void setBool(const std::string& name, bool value) const;
    
    /**
     * @brief Sets an integer uniform value.
     * @param name The name of the uniform variable in the shader.
     * @param value The integer value to set.
     */
    void setInt(const std::string& name, int value) const;
    
    /**
     * @brief Sets a float uniform value.
     * @param name The name of the uniform variable in the shader.
     * @param value The float value to set.
     */
    void setFloat(const std::string& name, float value) const;
    
    /**
     * @brief Sets a vec3 uniform value.
     * @param name The name of the uniform variable in the shader.
     * @param value The glm::vec3 value to set.
     */
    void setVec3(const std::string& name, const glm::vec3& value) const;
    
    /**
     * @brief Sets a vec4 uniform value.
     * @param name The name of the uniform variable in the shader.
     * @param value The glm::vec4 value to set.
     */
    void setVec4(const std::string& name, const glm::vec4& value) const;
    
    /**
     * @brief Sets a mat4 uniform value.
     * @param name The name of the uniform variable in the shader.
     * @param value The glm::mat4 value to set.
     */
    void setMat4(const std::string& name, const glm::mat4& value) const;

private:
    /**
     * @brief Cache for uniform locations to avoid repeated OpenGL queries.
     */
    mutable std::unordered_map<std::string, GLint> uniformLocationCache;
    
    /**
     * @brief Compiles a shader from source code.
     * 
     * @param source The shader source code.
     * @param type The shader type (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
     * @return The compiled shader object ID.
     * @throws ShaderException If compilation fails.
     */
    unsigned int compileShader(const char* source, unsigned int type);
    
    /**
     * @brief Links vertex and fragment shaders into a program.
     * 
     * @param vertexShader The compiled vertex shader ID.
     * @param fragmentShader The compiled fragment shader ID.
     * @throws ShaderException If linking fails.
     */
    void linkProgram(unsigned int vertexShader, unsigned int fragmentShader);
    
    /**
     * @brief Loads shader source code from a file.
     * 
     * @param filePath Path to the shader source file.
     * @return The contents of the file as a string.
     * @throws ShaderException If the file cannot be opened or is empty.
     */
    std::string loadShaderSource(const char* filePath);
    
    /**
     * @brief Gets the location of a uniform variable, using cache.
     * 
     * @param name The name of the uniform variable.
     * @return The uniform location, or -1 if not found.
     */
    GLint getUniformLocation(const std::string& name) const;
};

#endif
