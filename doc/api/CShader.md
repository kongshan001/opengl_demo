# CShader API 参考

`CShader` 类提供OpenGL着色器编译、链接和管理功能，支持多种Uniform变量类型。

## 类概述

```cpp
class CShader {
public:
    unsigned int ID;  // 着色器程序ID
    
    // 构造函数
    CShader(const char* vertexSource, const char* fragmentSource);
    CShader(const std::string& vertexPath, const std::string& fragmentPath);
    
    // 析造函数
    ~CShader();
    
    // 着色器使用
    void use();
    
    // Uniform设置方法
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
};
```

## 创建着色器

### 从文件加载（推荐）
```cpp
// 从文件创建着色器
CShader shader(
    "resources/shaders/vertex.glsl", 
    "resources/shaders/fragment.glsl"
);

// 使用着色器
shader.use();
```

### 从字符串创建
```cpp
// 从字符串创建着色器
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

CShader shader(vertexSource, fragmentSource);
```

## Uniform变量设置

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

### 材质Uniform设置示例
```cpp
shader.use();

// 设置漫反射颜色
shader.setVec3("material.diffuse", glm::vec3(0.8f, 0.3f, 0.2f));

// 设置镜面反射
shader.setFloat("material.shininess", 32.0f);
shader.setFloat("material.specularStrength", 0.5f);

// 设置纹理采样器
shader.setInt("texture_diffuse", 0);
shader.setInt("texture_normal", 1);
```

### MVP矩阵设置
```cpp
// 计算MVP矩阵
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                              (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                              0.1f, 100.0f);

// 设置到着色器
shader.use();
shader.setMat4("model", model);
shader.setMat4("view", view);
shader.setMat4("projection", projection);
```

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
    // 环境光
    vec3 ambient = 0.1 * lightColor;
    
    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // 纹理
    vec4 texColor = texture(texture_diffuse, TexCoord);
    
    FragColor = vec4((ambient + diffuse) * texColor.rgb, texColor.a);
}
```

## 错误处理

`CShader`类自动处理着色器编译和链接错误，错误信息会输出到标准输出：

```cpp
// 编译错误示例
Shader compilation failed: ERROR: 0:1: '' :  #version required and missing.
ERROR: 0:1: 'resources' : syntax error: syntax error

// 链接错误示例  
Program linking failed: ERROR: 0:1: '' :  #version required and missing.
ERROR: 0:1: 'resources' : syntax error: syntax error
```

## 最佳实践

### 1. 资源文件路径
```cpp
// ✅ 正确：使用绝对路径或相对路径从可执行文件位置
CShader shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");

// ✅ 正确：使用std::string构造函数
CShader shader(std::string("vertex.glsl"), std::string("fragment.glsl"));

// ❌ 错误：混合路径类型
CShader shader("vertex.glsl", std::string("../fragment.glsl"));  // 可能导致文件找不到
```

### 2. 着色器错误检查
- 确保#version指令在文件开头
- 检查分号使用情况
- 验证变量名称与使用一致
- 检查数据类型匹配

### 3. 性能优化
- 重新编译着色器仅在源代码改变时
- 缓存Uniform位置，避免频繁设置
- 使用glUniform*iv批量设置数组Uniform

## 常见问题

### 1. 着色器找不到
**问题：** 着色器文件路径错误
**解决：** 检查文件路径和CMakeLists.txt中的资源复制配置

### 2. Uniform未生效
**问题：** 在设置Uniform前忘记调用`use()`
**解决：** 确保在设置Uniform前调用`shader.use()`

### 3. 着色器编译错误
**问题：** GLSL语法错误或不支持的特性
**解决：** 使用glslc工具或在线GLSL编辑器检查语法

### 4. Uniform类型不匹配
**问题：** 设置了错误的Uniform类型
**解决：** 确认着色器中Uniform声明类型与C++中设置的类型一致