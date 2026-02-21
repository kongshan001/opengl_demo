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
    // 创建着色器（使用纹理着色器）
    try {
        shader = std::make_shared<CShader>(
            std::string("resources/shaders/mesh.vs"),
            std::string("resources/shaders/texture.fs")
        );
    } catch (const ShaderException& e) {
        std::cerr << "Shader error: " << e.what() << std::endl;
        return;
    }

    // 创建材质
    material = std::make_shared<CMaterial>("TexturedMaterial");
    material->setShader(shader);
    material->setColors(glm::vec3(1.0f), glm::vec3(0.5f), glm::vec3(0.1f));
    material->setProperties(32.0f, 0.5f);

    // 加载纹理
    try {
        diffuseTexture = std::make_shared<CTexture>(
            "resources/textures/container2.png",
            TextureType::Diffuse
        );
        std::cout << "Loaded diffuse texture: "
                  << diffuseTexture->width << "x" << diffuseTexture->height
                  << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Texture load error: " << e.what() << std::endl;
    }

    // 创建带纹理坐标的立方体
    std::vector<Vertex> cubeVertices = {
        // 前面
        Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
        // 后面
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)),
        // 顶面
        Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        // 底面
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
        // 右面
        Vertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        // 左面
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f))
    };

    std::vector<unsigned int> cubeIndices = {
        0, 1, 2, 2, 3, 0,       // 前
        4, 5, 6, 6, 7, 4,       // 后
        8, 9, 10, 10, 11, 8,    // 顶
        12, 13, 14, 14, 15, 12, // 底
        16, 17, 18, 18, 19, 16, // 右
        20, 21, 22, 22, 23, 20  // 左
    };

    texturedCube = std::make_shared<CMesh>(cubeVertices, cubeIndices);
    texturedCube->setMaterial(material);

    // 创建简单三角形（无纹理）
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

    // 设置材质属性
    shader->setVec3("material.diffuse", material->diffuseColor);
    shader->setVec3("material.specular", material->specularColor);
    shader->setVec3("material.ambient", material->ambientColor);
    shader->setFloat("material.shininess", material->shininess);
    shader->setFloat("material.specularStrength", material->specularStrength);
    shader->setFloat("material.opacity", material->opacity);

    // 设置纹理
    bool hasDiffuse = diffuseTexture != nullptr;
    shader->setInt("hasDiffuseTexture", hasDiffuse ? 1 : 0);
    shader->setInt("hasSpecularTexture", 0);

    if (hasDiffuse) {
        glActiveTexture(GL_TEXTURE0);
        diffuseTexture->bind(0);
        shader->setInt("diffuseTexture", 0);
    }
}

void Application::renderScene() {
    if (!shader) return;

    glm::mat4 model = glm::mat4(1.0f);

    // 渲染三角形（无纹理，在左侧）
    if (triangleMesh) {
        shader->setInt("hasDiffuseTexture", 0);
        glm::mat4 triangleModel = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.0f));
        shader->setMat4("model", triangleModel);
        triangleMesh->draw();
    }

    // 渲染纹理立方体（在右侧，旋转）
    if (texturedCube && diffuseTexture) {
        shader->setInt("hasDiffuseTexture", 1);
        glActiveTexture(GL_TEXTURE0);
        diffuseTexture->bind(0);

        glm::mat4 cubeModel = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
        cubeModel = glm::rotate(cubeModel, (float)glfwGetTime(),
                                glm::vec3(0.5f, 1.0f, 0.0f));
        shader->setMat4("model", cubeModel);
        texturedCube->draw();
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
