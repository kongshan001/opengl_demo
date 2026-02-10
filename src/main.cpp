#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include "shader/Shader.h"
#include "mesh/Mesh.h"
#include "mesh/Material.h"
#include "mesh/ModelLoader.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 简单的相机控制
glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Demo - CMesh System", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 创建着色器
    CShader shader(std::string("resources/shaders/mesh.vs"), std::string("resources/shaders/mesh.fs"));
    
    // 创建材质
    auto material = std::make_shared<CMaterial>("DemoMaterial");
    material->setColors(glm::vec3(1.0f, 0.5f, 0.2f), glm::vec3(1.0f), glm::vec3(0.1f));
    material->setProperties(32.0f, 0.5f);
    
    // 创建三角形网格
    std::vector<Vertex> triangleVertices = {
        Vertex(glm::vec3( 0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f)),
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f))
    };
    
    CMesh triangleMesh(triangleVertices);
    triangleMesh.setMaterial(material);
    
    // 尝试加载立方体模型
    std::vector<std::shared_ptr<CMesh>> loadedMeshes;
    try {
        loadedMeshes = CModelLoader::load("resources/models/cube.obj");
        if (!loadedMeshes.empty()) {
            std::cout << "Successfully loaded cube model with " << loadedMeshes.size() << " mesh(es)" << std::endl;
            for (auto& mesh : loadedMeshes) {
                mesh->setMaterial(material);
            }
        } else {
            std::cout << "Failed to load cube model" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Exception loading model: " << e.what() << std::endl;
    }
    
    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 计算帧时间
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        // 渲染
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        
        // 设置简单的MVP矩阵
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                              (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                              0.1f, 100.0f);
        
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        shader.setVec3("viewPos", cameraPos);
        
        // 渲染三角形
        glm::mat4 triangleModel = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.0f));
        shader.setMat4("model", triangleModel);
        triangleMesh.draw();
        
        // 渲染加载的模型（如果有）
        if (!loadedMeshes.empty()) {
            glm::mat4 cubeModel = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
            cubeModel = glm::rotate(cubeModel, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
            shader.setMat4("model", cubeModel);
            
            for (auto& mesh : loadedMeshes) {
                mesh->draw();
            }
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // 清理资源
    glfwTerminate();
    return 0;
}