#include "core/Application.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh/ModelLoader.h"

Application::Application(const AppConfig& config)
    : config(config),
      window(nullptr),
      camera(glm::vec3(0.0f, 0.0f, 3.0f)),
      deltaTime(0.0f),
      lastFrame(0.0f) {
}

Application::~Application() {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

bool Application::initialize() {
    if (!initWindow()) {
        return false;
    }
    
    if (!initOpenGL()) {
        return false;
    }
    
    initScene();
    return true;
}

bool Application::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    window = glfwCreateWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    return true;
}

bool Application::initOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    glEnable(GL_DEPTH_TEST);
    return true;
}

void Application::initScene() {
    // 创建着色器
    try {
        shader = std::make_shared<CShader>(config.shaderVertex, config.shaderFragment);
    } catch (const ShaderException& e) {
        std::cerr << "Shader error: " << e.what() << std::endl;
        return;
    }
    
    // 创建材质
    material = std::make_shared<CMaterial>("DemoMaterial");
    material->setShader(shader);
    material->setColors(glm::vec3(1.0f, 0.5f, 0.2f),
                        glm::vec3(1.0f),
                        glm::vec3(0.1f));
    material->setProperties(32.0f, 0.5f);
    
    // 创建三角形
    std::vector<Vertex> triangleVertices = {
        Vertex(glm::vec3( 0.0f,  0.5f, 0.0f),
               glm::vec3(0.0f, 0.0f, 1.0f),
               glm::vec2(0.5f, 1.0f)),
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f),
               glm::vec3(0.0f, 0.0f, 1.0f),
               glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f, -0.5f, 0.0f),
               glm::vec3(0.0f, 0.0f, 1.0f),
               glm::vec2(1.0f, 0.0f))
    };
    triangleMesh = std::make_shared<CMesh>(triangleVertices);
    triangleMesh->setMaterial(material);
    
    // 加载模型
    try {
        modelMeshes = CModelLoader::load(config.modelPath);
        if (!modelMeshes.empty()) {
            std::cout << "Loaded model: " << config.modelPath
                      << " (" << modelMeshes.size() << " mesh(es))" << std::endl;
            for (auto& mesh : modelMeshes) {
                mesh->setMaterial(material);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Model load error: " << e.what() << std::endl;
    }
}

void Application::run() {
    while (!shouldClose()) {
        updateDeltaTime();
        processInput();
        render();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::close() {
    glfwSetWindowShouldClose(window, true);
}

bool Application::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Application::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        close();
        return;
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Forward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Backward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Left, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Right, deltaTime);
}

void Application::updateDeltaTime() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void Application::render() {
    glClearColor(config.backgroundColor.r,
                 config.backgroundColor.g,
                 config.backgroundColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    setGlobalUniforms();
    renderScene();
}

void Application::setGlobalUniforms() {
    if (!shader) return;
    
    shader->use();
    shader->setMat4("view", camera.getViewMatrix());
    shader->setMat4("projection",
        camera.getProjectionMatrix(config.width, config.height));
    shader->setVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
    shader->setVec3("viewPos", camera.getPosition());
}

void Application::renderScene() {
    if (!shader) return;
    
    glm::mat4 model = glm::mat4(1.0f);
    
    // 渲染三角形
    if (triangleMesh) {
        glm::mat4 triangleModel = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.0f));
        shader->setMat4("model", triangleModel);
        triangleMesh->draw();
    }
    
    // 渲染模型
    if (!modelMeshes.empty()) {
        glm::mat4 cubeModel = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
        cubeModel = glm::rotate(cubeModel, (float)glfwGetTime(),
                                glm::vec3(0.5f, 1.0f, 0.0f));
        shader->setMat4("model", cubeModel);
        
        for (auto& mesh : modelMeshes) {
            mesh->draw();
        }
    }
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->config.width = width;
        app->config.height = height;
        glViewport(0, 0, width, height);
    }
}
