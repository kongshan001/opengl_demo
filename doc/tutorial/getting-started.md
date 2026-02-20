# 快速入门指南

欢迎来到OpenGL Demo！本指南将帮助你从零开始使用本项目的完整网格管理系统。

## 🚀 环境准备

### 1. 编译项目
```bash
# 克隆项目
git clone https://github.com/kongshan001/opengl_demo.git
cd opengl_demo

# 构建项目
mkdir build && cd build
cmake ..
make
```

### 2. 运行程序
```bash
./opengl_demo
```

你应该看到一个窗口，左侧是橙色三角形，右侧是旋转的立方体。

## 🎯 你的第一个程序

### 基础三角形
让我们从最简单的例子开始——创建一个彩色三角形。

#### 1. 创建顶点数据
```cpp
#include "mesh/Mesh.h"

int main() {
    // 初始化GLFW和GLAD...
    
    // 定义三角形顶点（位置 + 纹理坐标）
    std::vector<Vertex> vertices = {
        Vertex(
            glm::vec3(-0.5f, -0.5f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),  // 法线向上
            glm::vec2(0.0f, 0.0f)             // 左下角纹理
        ),
        Vertex(
            glm::vec3(0.5f, -0.5f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec2(1.0f, 0.0f)              // 右下角纹理
        ),
        Vertex(
            glm::vec3(0.5f, 0.5f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec2(1.0f, 1.0f)              // 右上角纹理
        ),
        Vertex(
            glm::vec3(-0.5f, 0.5f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec2(0.0f, 1.0f)              // 左上角纹理
        )
    };
    
    // 创建网格
    CMesh triangle(vertices);
    
    // 渲染循环...
}
```

#### 2. 创建简单着色器
```cpp
// 简单的片段着色器 - 输出固定颜色
const char* fragmentSource = R"GLSL(
    #version 330 core
    out vec4 FragColor;
    
    void main() {
        FragColor = vec4(1.0, 0.8, 0.2, 1.0);  // 橙色
    }
)";

// 传递顶点位置的简单顶点着色器
const char* vertexSource = R"GLSL(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    
    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
)";

CShader shader(vertexSource, fragmentSource);
```

#### 3. 渲染网格
```cpp
while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    shader.use();
    triangle.draw();  // 自动设置VAO并渲染
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

## 🎨 纹理立方体

现在让我们创建一个带纹理的立方体，展示完整的材质系统。

### 1. 加载纹理
```cpp
#include "mesh/Texture.h"

// 加载纹理
auto texture = std::make_shared<CTexture>("resources/textures/box.jpg", TextureType::Diffuse);

// 设置纹理参数（可选）
texture->setWrapMode(GL_REPEAT, GL_REPEAT);
texture->setFilterMode(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
texture->generateMipmaps();
```

### 2. 创建材质
```cpp
#include "mesh/Material.h"

auto material = std::make_shared<CMaterial>("BoxMaterial");

// 设置材质属性
material->setColors(
    glm::vec3(0.8f, 0.8f, 0.8f),  // 漫反射
    glm::vec3(1.0f, 1.0f, 1.0f),  // 镜面反射
    glm::vec3(0.1f, 0.1f, 0.1f)   // 环境反射
);

material->setProperties(32.0f, 0.8f);  // 光泽度

// 添加纹理
material->addTexture(texture);
```

### 3. 创建立方体
```cpp
#include "mesh/MeshUtils.h"

// 使用内置函数创建立方体
auto cube = MeshUtils::createCube(2.0f);  // 边长为2.0的立方体

// 设置材质
cube.setMaterial(material);
```

### 4. 纹理着色器
```cpp
// 片段着色器
const char* fragmentSource = R"GLSL(
    #version 330 core
    in vec3 FragPos;
    in vec3 Normal;
    in vec2 TexCoord;
    
    struct Material {
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };
    
    uniform Material material;
    uniform sampler2D texture_diffuse;
    
    void main() {
        // 环境光
        vec3 ambient = material.diffuse * 0.1;
        
        // 漫反射
        vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0) - FragPos);
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * material.diffuse;
        
        // 镜面反射
        vec3 viewDir = normalize(-FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = spec * material.specular;
        
        // 纹理
        vec4 texColor = texture(texture_diffuse, TexCoord);
        
        FragColor = vec4(ambient + diffuse + specular, 1.0) * texColor;
    }
)";
```

## 🎮 控制和交互

让我们添加简单的相机控制，让用户能够控制视角。

### 1. 相机类实现
```cpp
class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    float yaw, pitch;
    
    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f));
    
    glm::mat4 getViewMatrix() const;
    void processKeyboard(GLFWwindow* window);
    void processMouseMovement(float xOffset, float yOffset);
    void updateCameraVectors();
};
```

### 2. 回调函数设置
```cpp
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    camera.processMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(yoffset);
}
```

### 3. 在渲染循环中使用
```cpp
glm::mat4 view = camera.getViewMatrix();
glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 
                                      (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                      0.1f, 100.0f);

shader.setMat4("view", view);
shader.setMat4("projection", projection);
```

## 📂 从文件加载3D模型

### 1. 准备OBJ文件
创建一个简单的立方体OBJ文件：
```obj
# Simple cube.obj
v -0.5 -0.5 -0.5
v  0.5 -0.5 -0.5
v  0.5  0.5 -0.5
v -0.5 0.5  0.5

vn 0.0 1.0 0.0
vn 0.0 1.0 0.0
vn 0.0 -1.0 0.0
vn -0.0 -1.0 0.0

vt 0.0 0.0
vt 1.0 0.0
vt 1.0 1.0

f 1/1/1 1/1 1/1
f 2/2/1 2/2 1/1
f 3/3/1 3/3 1/1
f 1/1/1 3/3/1 3/3
```

### 2. 加载模型
```cpp
#include "mesh/ModelLoader.h"

// 加载模型
auto meshes = CModelLoader::load("resources/models/cube.obj");

if (!meshes.empty()) {
    std::cout << "Successfully loaded model with " << meshes.size() << " mesh(es)" << std::endl;
    
    // 为每个网格设置材质
    for (auto& mesh : meshes) {
        mesh->setMaterial(material);
    }
}
```

### 3. 渲染模型
```cpp
for (auto& mesh : meshes) {
    mesh->draw();
}
```

## 🔧 常见问题解决

### 1. "shader not found" 错误
**原因：** 着色器文件路径不正确
**解决：**
```cpp
// 使用绝对路径
CShader shader("/path/to/shaders/vertex.glsl", "/path/to/shaders/fragment.glsl");

// 或者使用可执行文件相对路径
CShader shader("../resources/shaders/vertex.glsl", "../resources/shaders/model.frag");
```

### 2. "texture not found" 错误
**原因：** 纹理文件路径不正确
**解决：** 检查文件路径和权限

### 3. 窗口黑屏
**原因：** 可能是：
- 着色器编译错误
- VAO未正确设置
- 视点数据为空
- 模型加载失败

**调试方法：**
```cpp
// 检查着色器编译错误
int success;
char infoLog[512];
glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
if (!success) {
    glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
    std::cout << "Shader compilation error: " << infoLog << std::endl;
}
```

### 4. 程序崩溃
**原因：** 访问空指针或数组越界
**调试方法：** 使用调试器或添加断点

## 🚀 进阶主题

### 1. 多重渲染目标
```cpp
// 创建多个渲染目标
unsigned int FBO;
glGenFramebuffers(1, &FBO);
glBindFramebuffer(GL_FRAMEBUFFER, FBO);

// 创建颜色纹理和深度缓冲
unsigned int colorBuffer, depthBuffer;
glGenTextures(1, &colorBuffer);
glGenRenderbuffers(1, &depthBuffer);

// 配置渲染目标
glBindTexture(GL_TEXTURE_2D, colorBuffer);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

// 使用渲染目标
glBindFramebuffer(GL_FRAMEBUFFER, FBO);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBuffer, 0);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer);

// 渲染到纹理
```

### 2. 实例化渲染
```cpp
// 1000个立方体的实例化渲染
for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 100; j++) {
        glm::mat4 model = glm::translate(glm::vec3(i * 3.0f - 15.0f, j * 3.0f - 15.0f, 0.0f));
        shader.setMat4("model", model);
        cube.draw();
    }
}
```

### 3. 几何着色器
```cpp
// 细分着色器
#version 330 core
layout (triangles) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, -0.5, 1.0);
}
```

现在你已经掌握了OpenGL Demo的基础使用方法！继续学习 [材质和纹理](../tutorial/materials-textures.md) 来学习更高级的渲染技术。