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
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Demo - Material with Shader", NULL, NULL);
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
    
    glEnable(GL_DEPTH_TEST);
    
    // 创建着色器并绑定到材质
    auto shader = std::make_shared<CShader>(
        std::string("resources/shaders/mesh.vs"), 
        std::string("resources/shaders/mesh.fs")
    );
    
    // 创建材质并设置shader
    auto material = std::make_shared<CMaterial>("DemoMaterial");
    material->setShader(shader);  // Material现在持有shader引用
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
    
    // 加载立方体模型
    std::vector<std::shared_ptr<CMesh>> loadedMeshes;
    try {
        loadedMeshes = CModelLoader::load("resources/models/cube.obj");
        if (!loadedMeshes.empty()) {
            std::cout << "Successfully loaded cube model with " << loadedMeshes.size() << " mesh(es)" << std::endl;
            for (auto& mesh : loadedMeshes) {
                mesh->setMaterial(material);  // 所有mesh使用同一个material（共享shader）
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Exception loading model: " << e.what() << std::endl;
    }
    
    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // 设置全局uniform（在material应用前设置）
        shader->use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                              (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                              0.1f, 100.0f);
        
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        shader->setVec3("viewPos", cameraPos);
        
        // 渲染三角形 - draw()会自动使用material中的shader
        glm::mat4 triangleModel = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.0f));
        shader->setMat4("model", triangleModel);
        triangleMesh.draw();  // 自动应用shader和材质
        
        // 渲染立方体
        if (!loadedMeshes.empty()) {
            glm::mat4 cubeModel = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
            cubeModel = glm::rotate(cubeModel, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
            shader->setMat4("model", cubeModel);
            
            for (auto& mesh : loadedMeshes) {
                mesh->draw();  // 自动应用shader和材质
            }
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}