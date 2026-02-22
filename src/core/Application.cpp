#include "core/Application.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh/ModelLoader.h"
#include "mesh/MeshUtils.h"

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
    // 创建着色器（使用 Phong 光照着色器）
    try {
        shader = std::make_shared<CShader>(
            std::string("resources/shaders/mesh.vs"),
            std::string("resources/shaders/phong.fs")
        );
        
        // 光源着色器
        lightShader = std::make_shared<CShader>(
            std::string("resources/shaders/mesh.vs"),
            std::string("resources/shaders/light_source.fs")
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

    // 创建多种几何体（使用 MeshUtils）
    // 立方体
    texturedCube = MeshUtils::createCube(1.0f);
    texturedCube->setMaterial(material);
    
    // 球体
    sphereMesh = MeshUtils::createSphere(0.5f, 32);
    sphereMesh->setMaterial(material);
    
    // 圆柱体
    cylinderMesh = MeshUtils::createCylinder(0.3f, 1.0f, 32);
    cylinderMesh->setMaterial(material);
    
    // 圆锥体
    coneMesh = MeshUtils::createCone(0.4f, 0.8f, 32);
    coneMesh->setMaterial(material);
    
    // 光源指示器（小球）
    lightIndicator = MeshUtils::createSphere(0.1f, 16);
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
    
    // 数字键切换显示模式
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) displayMode = 0;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) displayMode = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) displayMode = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) displayMode = 3;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) displayMode = 4;
    
    // 5 键切换线框模式
    static bool key5Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !key5Pressed) {
        key5Pressed = true;
        wireframeMode = !wireframeMode;
        glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) {
        key5Pressed = false;
    }
    
    // L 键切换光源动画
    static bool lPressed = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lPressed) {
        lPressed = true;
        lightAnimationEnabled = !lightAnimationEnabled;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
        lPressed = false;
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
    
    // 设置材质（Phong 光照）
    shader->setVec3("material.ambient", material->ambientColor);
    shader->setVec3("material.diffuse", material->diffuseColor);
    shader->setVec3("material.specular", material->specularColor);
    shader->setFloat("material.shininess", material->shininess);
    
    // 设置光源（Phong 光照）
    shader->setVec3("light.position", lightPos);
    shader->setVec3("light.color", lightColor);
    shader->setFloat("light.ambientStrength", 0.15f);
    shader->setFloat("light.diffuseStrength", 0.7f);
    shader->setFloat("light.specularStrength", 1.0f);
    
    // 设置观察位置（用于镜面反射）
    shader->setVec3("viewPos", camera.getPosition());
}

void Application::renderScene() {
    if (!shader) return;

    // 获取当前时间（暂停时使用暂停时间）
    float currentTime = isPaused ? pausedTime : (float)glfwGetTime();
    
    // 更新光源位置（圆形轨道）
    if (lightAnimationEnabled) {
        lightPos.x = sin(currentTime * lightOrbitSpeed) * lightOrbitRadius;
        lightPos.z = cos(currentTime * lightOrbitSpeed) * lightOrbitRadius;
        lightPos.y = lightOrbitHeight;
        
        // 更新光源 uniform
        shader->use();
        shader->setVec3("light.position", lightPos);
    }

    shader->setInt("hasDiffuseTexture", 1);
    glActiveTexture(GL_TEXTURE0);
    diffuseTexture->bind(0);

    // 渲染多种几何体
    struct GeometryInfo {
        std::shared_ptr<CMesh> mesh;
        glm::vec3 position;
        glm::vec3 color;
        float rotationSpeed;
        float scale;
        int id;  // 用于显示模式过滤
    };
    
    std::vector<GeometryInfo> geometries = {
        { texturedCube,  glm::vec3(-2.5f, 0.0f, 0.0f), glm::vec3(1.0f, 0.8f, 0.8f), 0.3f, 0.8f, 1 },  // 立方体
        { sphereMesh,    glm::vec3( 0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 1.0f, 0.8f), 0.5f, 1.0f, 2 },  // 球体
        { cylinderMesh,  glm::vec3( 2.5f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 1.0f), 0.4f, 1.0f, 3 },  // 圆柱体
        { coneMesh,      glm::vec3( 0.0f, 1.5f,-2.0f), glm::vec3(1.0f, 1.0f, 0.8f), 0.6f, 1.0f, 4 }   // 圆锥体
    };
    
    for (const auto& geo : geometries) {
        if (!geo.mesh) continue;
        
        // 根据 displayMode 过滤
        if (displayMode != 0 && displayMode != geo.id) continue;
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, geo.position);
        model = glm::rotate(model, currentTime * geo.rotationSpeed,
                           glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(geo.scale));
        shader->setMat4("model", model);
        // 为每个几何体设置不同的材质颜色
        shader->setVec3("material.diffuse", geo.color);
        shader->setVec3("material.ambient", geo.color * 0.3f);
        geo.mesh->draw();
    }
    
    // 地面只在显示全部时渲染
    if (displayMode == 0) {
        shader->setInt("hasDiffuseTexture", 0);
        shader->setVec3("material.diffuse", glm::vec3(0.35f, 0.35f, 0.4f));
        shader->setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.12f));
        shader->setVec3("material.specular", glm::vec3(0.2f, 0.2f, 0.2f));
        
        glm::mat4 groundModel = glm::mat4(1.0f);
        groundModel = glm::translate(groundModel, glm::vec3(0.0f, -0.75f, 0.0f));
        groundModel = glm::scale(groundModel, glm::vec3(10.0f, 0.1f, 10.0f));
        shader->setMat4("model", groundModel);
        texturedCube->draw();
    }
    
    // 渲染光源指示器
    if (lightShader && lightIndicator) {
        lightShader->use();
        lightShader->setMat4("view", camera.getViewMatrix());
        lightShader->setMat4("projection", camera.getProjectionMatrix(config.width, config.height));
        lightShader->setVec3("lightColor", lightColor);
        
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightShader->setMat4("model", lightModel);
        lightIndicator->draw();
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
