#include "core/Application.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh/ModelLoader.h"

Application::Application(const AppConfig& config)
    : config(config),
      window(nullptr),
      camera(glm::vec3(0.0f, 0.0f, 4.0f)),  // 摄像机后退，确保立方体完整可见
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
    
    // 注册鼠标回调
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    // 捕获鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    // 创建着色器（使用简化纹理着色器）
    try {
        shader = std::make_shared<CShader>(
            std::string("resources/shaders/mesh.vs"),
            std::string("resources/shaders/texture_simple.fs")
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

        // FPS 计算
        frameCount++;
        fpsTimer += deltaTime;
        if (fpsTimer >= 1.0f) {
            currentFPS = frameCount / fpsTimer;
            float frameTime = 1000.0f / currentFPS;
            std::string title = config.title + 
                " | FPS: " + std::to_string(static_cast<int>(currentFPS)) +
                " | Frame: " + std::to_string(static_cast<int>(frameTime)) + "ms";
            if (isPaused) {
                title += " [PAUSED]";
            }
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            fpsTimer = 0.0f;
        }

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

    // WASD + QE 移动控制
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Forward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Backward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Left, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Right, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Down, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Up, deltaTime);
    
    // 空格键暂停/继续 (使用按键释放检测，避免连续触发)
    static bool spacePressed = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
        spacePressed = true;
        isPaused = !isPaused;
        if (isPaused) {
            pausedTime = glfwGetTime();
        } else {
            // 恢复时调整 lastFrame 避免时间跳跃
            lastFrame = glfwGetTime();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        spacePressed = false;
    }
    
    // R 键重置摄像机
    static bool rPressed = false;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rPressed) {
        rPressed = true;
        camera.setPosition(glm::vec3(0.0f, 0.0f, 4.0f));
        camera.setMovementSpeed(2.5f);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
        rPressed = false;
    }
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

    // 设置纹理
    bool hasDiffuse = diffuseTexture != nullptr;
    shader->setInt("hasDiffuseTexture", hasDiffuse ? 1 : 0);

    if (hasDiffuse) {
        glActiveTexture(GL_TEXTURE0);
        diffuseTexture->bind(0);
        shader->setInt("diffuseTexture", 0);
    }
    
    // 设置材质颜色（无纹理时使用）
    shader->setVec3("materialDiffuse", material->diffuseColor);
}

void Application::renderScene() {
    if (!shader) return;

    // 获取当前时间（暂停时使用暂停时间）
    float currentTime = isPaused ? pausedTime : (float)glfwGetTime();

    shader->setInt("hasDiffuseTexture", 1);
    glActiveTexture(GL_TEXTURE0);
    diffuseTexture->bind(0);

    // 渲染多个立方体，不同位置和颜色
    struct CubeInfo {
        glm::vec3 position;
        float rotationSpeed;
        glm::vec3 color;  // 用于区分
    };
    
    CubeInfo cubes[] = {
        { glm::vec3( 0.0f,  0.0f,  0.0f), 0.3f, glm::vec3(1.0f, 1.0f, 1.0f) },
        { glm::vec3( 2.0f,  0.0f, -1.0f), 0.5f, glm::vec3(1.0f, 0.8f, 0.8f) },
        { glm::vec3(-2.0f,  0.0f, -1.0f), 0.2f, glm::vec3(0.8f, 1.0f, 0.8f) },
        { glm::vec3( 0.0f,  1.5f, -2.0f), 0.4f, glm::vec3(0.8f, 0.8f, 1.0f) }
    };
    
    for (const auto& cube : cubes) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cube.position);
        model = glm::rotate(model, currentTime * cube.rotationSpeed,
                           glm::vec3(0.5f, 1.0f, 0.3f));
        shader->setMat4("model", model);
        shader->setVec3("materialDiffuse", cube.color);
        texturedCube->draw();
    }
    
    // 渲染地面（一个大平面）
    shader->setInt("hasDiffuseTexture", 0);
    shader->setVec3("materialDiffuse", glm::vec3(0.4f, 0.4f, 0.4f));
    
    glm::mat4 groundModel = glm::mat4(1.0f);
    groundModel = glm::translate(groundModel, glm::vec3(0.0f, -0.5f, 0.0f));
    groundModel = glm::scale(groundModel, glm::vec3(10.0f, 0.1f, 10.0f));
    shader->setMat4("model", groundModel);
    texturedCube->draw();
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->config.width = width;
        app->config.height = height;
        glViewport(0, 0, width, height);
    }
}

void Application::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app) return;
    
    if (app->firstMouse) {
        app->lastMouseX = static_cast<float>(xpos);
        app->lastMouseY = static_cast<float>(ypos);
        app->firstMouse = false;
    }
    
    float xoffset = static_cast<float>(xpos) - app->lastMouseX;
    float yoffset = app->lastMouseY - static_cast<float>(ypos);  // 反转Y轴
    
    app->lastMouseX = static_cast<float>(xpos);
    app->lastMouseY = static_cast<float>(ypos);
    
    app->camera.processMouseMovement(xoffset, yoffset);
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->camera.processMouseScroll(static_cast<float>(yoffset));
    }
}
