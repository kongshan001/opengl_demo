# CShader API 参考

`CShader` 类提供 OpenGL 着色器编译、链接和管理功能，支持多种 Uniform 变量类型。

## 异常类

```cpp
class ShaderException : public std::exception {
public:
    explicit ShaderException(const std::string& message);
    const char* what() const noexcept override;
};
```

当以下情况发生时抛出 `ShaderException`：
- 着色器文件无法打开
- 着色器文件为空
- 着色器编译失败
- 着色器程序链接失败

## 类概述

```cpp
class CShader {
public:
    unsigned int ID;  // 着色器程序 ID
    
    // 构造函数（失败时抛出 ShaderException）
    CShader(const char* vertexSource, const char* fragmentSource);
    CShader(const std::string& vertexPath, const std::string& fragmentPath);
    
    // 析构函数（RAII：自动删除程序）
    ~CShader();
    
    // 激活着色器
    void use();
    
    // Uniform 设置方法（自动缓存 location）
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
};
```

## 特性

### Uniform Location 缓存
Uniform location 会自动缓存，避免重复的 OpenGL 查询，提升性能：

```cpp
// 首次调用：查询 OpenGL 并缓存
shader.setMat4("model", modelMatrix);

// 后续调用：直接使用缓存
shader.setMat4("model", anotherMatrix);  // 更快！
```

### RAII 资源管理
着色器程序在析构时自动删除：

```cpp
{
    CShader shader("vs.glsl", "fs.glsl");
    // 使用 shader...
}  // 自动调用 glDeleteProgram()
```

## 创建着色器

### 从文件加载（推荐）
```cpp
try {
    CShader shader(
        "resources/shaders/vertex.glsl", 
        "resources/shaders/fragment.glsl"
    );
    shader.use();
} catch (const ShaderException& e) {
    std::cerr << "Shader error: " << e.what() << std::endl;
}
```

### 从字符串创建
```cpp
const char* vertexSource = R"GLSL(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
)";

const char* fragmentSource = R"GLSL(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    }
)";

try {
    CShader shader(vertexSource, fragmentSource);
} catch (const ShaderException& e) {
    // 处理编译/链接错误
}
```

## Uniform 变量设置

### 基础类型
```cpp
shader.use();

// 布尔值
shader.setBool("useLighting", true);
shader.setInt("textureUnit", 0);
shader.setFloat("alpha", 0.8f);

// 向量
shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
shader.setVec4("color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));

// 矩阵
glm::mat4 model = glm::mat4(1.0f);
shader.setMat4("model", model);
```

### MVP 矩阵设置
```cpp
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                              (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                              0.1f, 100.0f);

shader.use();
shader.setMat4("model", model);
shader.setMat4("view", view);
shader.setMat4("projection", projection);
```

## 错误处理

### 捕获异常
```cpp
try {
    CShader shader("nonexistent.glsl", "fragment.glsl");
} catch (const ShaderException& e) {
    // 输出：Failed to open shader file: nonexistent.glsl
    std::cerr << e.what() << std::endl;
}
```

### 常见错误信息

| 错误类型 | 示例信息 |
|---------|---------|
| 文件未找到 | `Failed to open shader file: path/to/shader.glsl` |
| 文件为空 | `Shader file is empty: path/to/shader.glsl` |
| 编译失败 | `Vertex shader compilation failed: ERROR: 0:1: ...` |
| 链接失败 | `Shader program linking failed: ERROR: ...` |

## 着色器文件规范

### 顶点着色器示例
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
```

### 片段着色器示例
```glsl
#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_diffuse;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main() {
    vec3 ambient = 0.1 * lightColor;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec4 texColor = texture(texture_diffuse, TexCoord);
    
    FragColor = vec4((ambient + diffuse) * texColor.rgb, texColor.a);
}
```

## 最佳实践

### 1. 资源文件路径
```cpp
// ✅ 正确：使用相对路径
CShader shader("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl");

// ❌ 错误：使用不存在的路径（会抛出异常）
CShader shader("missing.glsl", "missing.fs");  // ShaderException!
```

### 2. 异常处理
```cpp
// ✅ 正确：捕获异常
try {
    CShader shader(path1, path2);
    // 使用 shader...
} catch (const ShaderException& e) {
    logError(e.what());
    return false;
}

// ❌ 错误：忽略异常可能导致程序崩溃
CShader shader("bad.glsl", "bad.fs");  // 未捕获的异常
```

### 3. 性能优化
- Uniform location 自动缓存，无需手动优化
- 避免每帧重新创建着色器对象
- 在初始化时加载所有着色器

## 常见问题

### 1. 着色器文件找不到
**问题：** `ShaderException: Failed to open shader file`
**解决：** 检查文件路径，确保资源已复制到构建目录

### 2. Uniform 未生效
**问题：** 设置了 Uniform 但效果不显示
**解决：** 确保在设置前调用 `shader.use()`

### 3. 编译错误
**问题：** `Vertex shader compilation failed`
**解决：** 检查 GLSL 语法，确保 `#version` 在文件开头
