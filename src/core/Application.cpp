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
    // 使用兼容模式，以便在更多环境中工作
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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
    // Create basic shader (simple texture)
    try {
        shader = std::make_shared<CShader>(
            std::string("resources/shaders/mesh.vs"),
            std::string("resources/shaders/texture_simple.fs")
        );
    } catch (const ShaderException& e) {
        std::cerr << "Shader error: " << e.what() << std::endl;
        return;
    }

    // Create lighting shader (multi-light support)
    try {
        lightingShader = std::make_shared<CShader>(
            std::string("resources/shaders/lighting.vs"),
            std::string("resources/shaders/lighting.fs")
        );
        std::cout << "Lighting shader loaded successfully" << std::endl;
    } catch (const ShaderException& e) {
        std::cerr << "Lighting shader error: " << e.what() << std::endl;
    }

    // Create shadow depth shader
    try {
        shadowShader = std::make_shared<CShader>(
            std::string("resources/shaders/shadow_depth.vs"),
            std::string("resources/shaders/shadow_depth.fs")
        );
        std::cout << "Shadow shader loaded successfully" << std::endl;
    } catch (const ShaderException& e) {
        std::cerr << "Shadow shader error: " << e.what() << std::endl;
    }

    // Initialize shadow mapper
    shadowMapper = std::make_unique<ShadowMapper>();
    if (shadowMapper->initialize()) {
        std::cout << "Shadow mapper initialized (1024x1024)" << std::endl;
    } else {
        std::cerr << "Failed to initialize shadow mapper" << std::endl;
    }

    // Initialize lights
    initLights();
    
    // Initialize particles
    initParticles();
    
    // Initialize skybox
    initSkybox();
    
    // Create material
    material = std::make_shared<CMaterial>("TexturedMaterial");
    // 不设置材质的shader，让Mesh::draw()使用当前激活的shader（lightingShader）
    // material->setShader(shader);  // 注释掉这行
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
    
    std::cout << "Textured cube created with " << cubeVertices.size() 
              << " vertices and " << cubeIndices.size() << " indices" << std::endl;

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
        camera.update(deltaTime);  // Update camera (auto-rotate, etc.)
        
        // Update particles
        if (particleEmitter_ && particlesEnabled_) {
            particleEmitter_->update(deltaTime);
        }
        
        render();

        // FPS calculation
        frameCount++;
        fpsTimer += deltaTime;
        if (fpsTimer >= 1.0f) {
            currentFPS = frameCount / fpsTimer;
            float frameTime = 1000.0f / currentFPS;
            std::string title = config.title +
                " | FPS: " + std::to_string(static_cast<int>(currentFPS)) +
                " | Frame: " + std::to_string(static_cast<int>(frameTime)) + "ms" +
                " | Camera: " + camera.getModeName();
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

    // Camera mode switching (1-4 keys)
    static bool key1Pressed = false;
    static bool key2Pressed = false;
    static bool key3Pressed = false;
    static bool key4Pressed = false;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !key1Pressed) {
        key1Pressed = true;
        camera.setMode(CameraMode::FirstPerson);
        std::cout << "Camera mode: First Person" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) key1Pressed = false;

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !key2Pressed) {
        key2Pressed = true;
        camera.setMode(CameraMode::ThirdPerson);
        camera.setTarget(glm::vec3(0.0f));
        camera.setOrbitDistance(5.0f);
        std::cout << "Camera mode: Third Person" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) key2Pressed = false;

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !key3Pressed) {
        key3Pressed = true;
        camera.setMode(CameraMode::FreeFlight);
        std::cout << "Camera mode: Free Flight" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) key3Pressed = false;

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !key4Pressed) {
        key4Pressed = true;
        camera.setMode(CameraMode::Orbit);
        camera.setTarget(glm::vec3(0.0f));
        camera.setOrbitDistance(8.0f);
        camera.setAutoRotate(true);
        camera.setAutoRotateSpeed(15.0f);
        std::cout << "Camera mode: Orbit (auto-rotate)" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) key4Pressed = false;

    // Tab key to cycle camera mode
    static bool tabPressed = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
        tabPressed = true;
        camera.cycleMode();
        std::cout << "Camera mode: " << camera.getModeName() << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) tabPressed = false;

    // O key to toggle orbit auto-rotation
    static bool oPressed = false;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !oPressed) {
        oPressed = true;
        if (camera.getMode() == CameraMode::Orbit) {
            bool currentState = camera.isAutoRotate();
            camera.setAutoRotate(!currentState);
            std::cout << "Auto-rotate: " << (!currentState ? "ON" : "OFF") << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) oPressed = false;

    // Light controls (L key cycles through lights, +/- adjusts intensity)
    static bool lPressed = false;
    static int selectedLightIndex = 0;

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lPressed) {
        lPressed = true;
        selectedLightIndex = (selectedLightIndex + 1) % static_cast<int>(lightManager.getLightCount());
        auto light = lightManager.getLight(selectedLightIndex);
        if (light) {
            std::cout << "Selected light " << selectedLightIndex << ": " << light->getName() << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) lPressed = false;

    // +/- to adjust light intensity
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        auto light = lightManager.getLight(selectedLightIndex);
        if (light) {
            float newIntensity = light->getIntensity() + 0.5f * deltaTime;
            light->setIntensity(std::min(newIntensity, 5.0f));
        }
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        auto light = lightManager.getLight(selectedLightIndex);
        if (light) {
            float newIntensity = light->getIntensity() - 0.5f * deltaTime;
            light->setIntensity(std::max(newIntensity, 0.0f));
        }
    }

    // T key to toggle selected light
    static bool tPressed = false;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !tPressed) {
        tPressed = true;
        auto light = lightManager.getLight(selectedLightIndex);
        if (light) {
            light->setEnabled(!light->isEnabled());
            std::cout << "Light " << selectedLightIndex << ": " << (light->isEnabled() ? "ON" : "OFF") << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) tPressed = false;

    // P key to toggle shadows
    static bool pPressed = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pPressed) {
        pPressed = true;
        shadowsEnabled_ = !shadowsEnabled_;
        std::cout << "Shadows: " << (shadowsEnabled_ ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) pPressed = false;

    // F key to toggle PCF (soft shadows)
    static bool fPressed = false;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fPressed) {
        fPressed = true;
        if (shadowMapper) {
            shadowMapper->setPCFEnabled(!shadowMapper->isPCFEnabled());
            std::cout << "PCF (soft shadows): " << (shadowMapper->isPCFEnabled() ? "ON" : "OFF") << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) fPressed = false;

    // G key to toggle particles
    static bool gPressed = false;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gPressed) {
        gPressed = true;
        particlesEnabled_ = !particlesEnabled_;
        std::cout << "Particles: " << (particlesEnabled_ ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) gPressed = false;

    // H key to cycle particle presets
    static bool hPressed = false;
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !hPressed) {
        hPressed = true;
        cycleParticlePreset();
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE) hPressed = false;

    // B key for burst (explosion)
    static bool bPressed = false;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bPressed) {
        bPressed = true;
        if (particleEmitter_) {
            particleEmitter_->burst();
            std::cout << "Particle burst!" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) bPressed = false;
    
    // N key to cycle skybox presets
    static bool nPressed = false;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !nPressed) {
        nPressed = true;
        cycleSkyboxPreset();
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE) nPressed = false;
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

    // Shadow pass (render to depth map)
    if (shadowsEnabled_ && shadowMapper && shadowShader) {
        renderShadowPass();
    }

    // Normal render pass
    glViewport(0, 0, config.width, config.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 确保深度测试启用
    glEnable(GL_DEPTH_TEST);

    setGlobalUniforms();
    renderScene();
    
    // 检查OpenGL错误
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        static bool errorOnce = true;
        if (errorOnce) {
            std::cout << "OpenGL error after render: " << err << std::endl;
            errorOnce = false;
        }
    }
}

void Application::setGlobalUniforms() {
    if (!shader) return;

    // Basic shader for simple rendering
    shader->use();
    shader->setMat4("view", camera.getViewMatrix());
    shader->setMat4("projection",
        camera.getProjectionMatrix(config.width, config.height));

    bool hasDiffuse = diffuseTexture != nullptr;
    shader->setInt("hasDiffuseTexture", hasDiffuse ? 1 : 0);

    if (hasDiffuse) {
        glActiveTexture(GL_TEXTURE0);
        diffuseTexture->bind(0);
        shader->setInt("diffuseTexture", 0);
    }

    shader->setVec3("materialDiffuse", material->diffuseColor);

    // Lighting shader uniforms
    if (!lightingShader) return;

    lightingShader->use();
    lightingShader->setMat4("view", camera.getViewMatrix());
    lightingShader->setMat4("projection",
        camera.getProjectionMatrix(config.width, config.height));

    // Set camera position for specular calculations
    lightingShader->setVec3("viewPos", camera.getPosition());

    // Set ambient color
    lightingShader->setVec3("ambientColor", lightManager.getAmbientColor());

    // Set material properties
    lightingShader->setVec3("material.ambient", material->ambientColor);
    lightingShader->setVec3("material.diffuse", material->diffuseColor);
    lightingShader->setVec3("material.specular", material->specularColor);
    lightingShader->setFloat("material.shininess", material->shininess);

    // Set texture
    lightingShader->setInt("hasDiffuseTexture", hasDiffuse ? 1 : 0);
    if (hasDiffuse) {
        glActiveTexture(GL_TEXTURE0);
        diffuseTexture->bind(0);
        lightingShader->setInt("diffuseTexture", 0);
    }

    // Set light data
    int numLights = lightManager.getEnabledLightCount();
    lightingShader->setInt("numLights", numLights);

    // Update spotlight position to follow camera (if enabled)
    auto spotlight = lightManager.getLight("spotlight");
    if (spotlight && spotlight->isEnabled()) {
        auto spot = std::dynamic_pointer_cast<SpotLight>(spotlight);
        if (spot) {
            spot->setPosition(camera.getPosition());
            spot->setDirection(camera.getFront());
        }
    }

    // Pass light data to shader
    int lightIndex = 0;
    lightManager.forEachEnabledLight([&](const Light* light, int index) {
        glm::vec3 pos, dir, color;
        float intensity, constant, linear, quadratic, innerCutoff, outerCutoff;
        light->getShaderData(pos, dir, color, intensity, constant, linear, quadratic, innerCutoff, outerCutoff);

        std::string prefix = "lights[" + std::to_string(lightIndex) + "].";

        lightingShader->setVec3(prefix + "position", pos);
        lightingShader->setVec3(prefix + "direction", dir);
        lightingShader->setVec3(prefix + "color", color);
        lightingShader->setFloat(prefix + "intensity", intensity);
        lightingShader->setFloat(prefix + "constant", constant);
        lightingShader->setFloat(prefix + "linear", linear);
        lightingShader->setFloat(prefix + "quadratic", quadratic);
        lightingShader->setFloat(prefix + "innerCutoff", innerCutoff);
        lightingShader->setFloat(prefix + "outerCutoff", outerCutoff);

        // Light type: 0=directional, 1=point, 2=spotlight
        int type = 0;
        if (light->getType() == LightType::Point) type = 1;
        else if (light->getType() == LightType::Spotlight) type = 2;
        lightingShader->setInt(prefix + "type", type);

        lightIndex++;
    });

    // Render particles
    if (particleEmitter_ && particlesEnabled_) {
        particleRenderer_->render(*particleEmitter_, camera.getViewMatrix(),
                           camera.getProjectionMatrix(config.width, config.height));
    }

    // Set shadow uniforms
    if (shadowMapper && shadowsEnabled_) {
        lightingShader->setMat4("lightSpaceMatrix", shadowMapper->getLightSpaceMatrix());
        lightingShader->setInt("shadowsEnabled", 1);
        lightingShader->setFloat("shadowBiasMin", shadowMapper->getBiasMin());
        lightingShader->setFloat("shadowBiasMax", shadowMapper->getBiasMax());
        lightingShader->setInt("pcfEnabled", shadowMapper->isPCFEnabled() ? 1 : 0);

        // Bind shadow map to texture unit 1
        shadowMapper->bindShadowMap(1);
        lightingShader->setInt("shadowMap", 1);
    } else {
        // 即使阴影禁用，也需要设置lightSpaceMatrix，因为顶点着色器总是使用它
        lightingShader->setMat4("lightSpaceMatrix", glm::mat4(1.0f));
        lightingShader->setInt("shadowsEnabled", 0);
    }
}

void Application::renderScene() {
    // Use lighting shader for main scene
    if (!lightingShader) {
        // Fallback to basic shader
        if (!shader) return;
        shader->use();
        renderSimpleScene();
        return;
    }

    renderLitScene();
}

void Application::renderSimpleScene() {
    float currentTime = isPaused ? pausedTime : (float)glfwGetTime();

    shader->setInt("hasDiffuseTexture", 1);
    glActiveTexture(GL_TEXTURE0);
    diffuseTexture->bind(0);

    struct CubeInfo {
        glm::vec3 position;
        float rotationSpeed;
        glm::vec3 color;
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

    shader->setInt("hasDiffuseTexture", 0);
    shader->setVec3("materialDiffuse", glm::vec3(0.4f, 0.4f, 0.4f));

    glm::mat4 groundModel = glm::mat4(1.0f);
    groundModel = glm::translate(groundModel, glm::vec3(0.0f, -0.5f, 0.0f));
    groundModel = glm::scale(groundModel, glm::vec3(10.0f, 0.1f, 10.0f));
    shader->setMat4("model", groundModel);
    texturedCube->draw();
}

void Application::renderLitScene() {
    float currentTime = isPaused ? pausedTime : (float)glfwGetTime();

    lightingShader->use();
    
    // Debug: 检查着色器是否有效
    static bool debugOnce = true;
    if (debugOnce) {
        std::cout << "renderLitScene called" << std::endl;
        std::cout << "Camera position: " << camera.getPosition().x << ", " 
                  << camera.getPosition().y << ", " << camera.getPosition().z << std::endl;
        debugOnce = false;
    }
    
    // 恢复纹理设置
    bool hasDiffuse = diffuseTexture != nullptr;
    lightingShader->setInt("hasDiffuseTexture", hasDiffuse ? 1 : 0);
    if (hasDiffuse) {
        glActiveTexture(GL_TEXTURE0);
        diffuseTexture->bind(0);
        lightingShader->setInt("diffuseTexture", 0);
    }

    // Render cubes with lighting
    struct CubeInfo {
        glm::vec3 position;
        float rotationSpeed;
    };

    CubeInfo cubes[] = {
        { glm::vec3( 0.0f,  0.0f,  0.0f), 0.3f },
        { glm::vec3( 2.0f,  0.0f, -1.0f), 0.5f },
        { glm::vec3(-2.0f,  0.0f, -1.0f), 0.2f },
        { glm::vec3( 0.0f,  1.5f, -2.0f), 0.4f }
    };

    for (const auto& cube : cubes) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cube.position);
        model = glm::rotate(model, currentTime * cube.rotationSpeed,
                           glm::vec3(0.5f, 1.0f, 0.3f));
        lightingShader->setMat4("model", model);
        texturedCube->draw();
    }

    // Render ground
    lightingShader->setInt("hasDiffuseTexture", 0);
    lightingShader->setVec3("material.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));

    glm::mat4 groundModel = glm::mat4(1.0f);
    groundModel = glm::translate(groundModel, glm::vec3(0.0f, -0.5f, 0.0f));
    groundModel = glm::scale(groundModel, glm::vec3(10.0f, 0.1f, 10.0f));
    lightingShader->setMat4("model", groundModel);
    texturedCube->draw();

    // Render skybox (render last to avoid depth test issues)
    if (skybox_ && skyboxEnabled_) {
        skybox_->render(camera.getViewMatrix(),
                         camera.getProjectionMatrix(config.width, config.height));
    }

    // Render point light indicators (small spheres)
    renderLightIndicators();
}

void Application::renderLightIndicators() {
    // Use simple shader for light indicators
    if (!shader) return;
    shader->use();
    shader->setMat4("view", camera.getViewMatrix());
    shader->setMat4("projection", camera.getProjectionMatrix(config.width, config.height));
    shader->setInt("hasDiffuseTexture", 0);

    // Draw small cubes at point light positions
    auto pointLights = lightManager.getPointLights();
    for (const auto& light : pointLights) {
        if (!light->isEnabled()) continue;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light->getPosition());
        model = glm::scale(model, glm::vec3(0.2f));
        shader->setMat4("model", model);
        shader->setVec3("materialDiffuse", light->getColor());
        texturedCube->draw();
    }
}

void Application::renderShadowPass() {
    if (!shadowMapper || !shadowShader || !texturedCube) return;

    // Get directional light for shadow calculation
    auto dirLights = lightManager.getDirectionalLights();
    if (dirLights.empty()) return;

    auto sun = dirLights[0];
    if (!sun->isEnabled()) return;

    // Update light space matrix
    shadowMapper->updateLightSpaceMatrix(sun->getDirection(), glm::vec3(0.0f, 0.0f, -1.0f));

    // Begin shadow pass
    shadowMapper->beginPass();

    // Use shadow shader
    shadowShader->use();
    shadowShader->setMat4("lightSpaceMatrix", shadowMapper->getLightSpaceMatrix());

    // Render scene geometry (depth only)
    float currentTime = isPaused ? pausedTime : (float)glfwGetTime();

    // Render cubes
    struct CubeInfo {
        glm::vec3 position;
        float rotationSpeed;
    };

    CubeInfo cubes[] = {
        { glm::vec3( 0.0f,  0.0f,  0.0f), 0.3f },
        { glm::vec3( 2.0f,  0.0f, -1.0f), 0.5f },
        { glm::vec3(-2.0f,  0.0f, -1.0f), 0.2f },
        { glm::vec3( 0.0f,  1.5f, -2.0f), 0.4f }
    };

    for (const auto& cube : cubes) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cube.position);
        model = glm::rotate(model, currentTime * cube.rotationSpeed,
                           glm::vec3(0.5f, 1.0f, 0.3f));
        shadowShader->setMat4("model", model);
        texturedCube->draw();
    }

    // Render ground
    glm::mat4 groundModel = glm::mat4(1.0f);
    groundModel = glm::translate(groundModel, glm::vec3(0.0f, -0.5f, 0.0f));
    groundModel = glm::scale(groundModel, glm::vec3(10.0f, 0.1f, 10.0f));
    shadowShader->setMat4("model", groundModel);
    texturedCube->draw();

    // End shadow pass
    shadowMapper->endPass();
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

void Application::initLights() {
    // Set ambient color
    lightManager.setAmbientColor(glm::vec3(0.1f, 0.1f, 0.12f));

    // Directional light (sun)
    auto sun = std::make_shared<DirectionalLight>("sun");
    sun->setColor(glm::vec3(1.0f, 0.95f, 0.9f));  // Warm white
    sun->setIntensity(1.0f);
    sun->setDirection(glm::vec3(-0.5f, -1.0f, -0.3f));
    lightManager.addLight(sun);
    std::cout << "Added directional light: sun" << std::endl;

    // Point light 1 (red, position left)
    auto pointLight1 = std::make_shared<PointLight>("point_red");
    pointLight1->setPosition(glm::vec3(-3.0f, 2.0f, 0.0f));
    pointLight1->setColor(glm::vec3(1.0f, 0.3f, 0.3f));
    pointLight1->setIntensity(0.8f);
    pointLight1->setAttenuation(1.0f, 0.09f, 0.032f);
    lightManager.addLight(pointLight1);
    std::cout << "Added point light: point_red" << std::endl;

    // Point light 2 (blue, position right)
    auto pointLight2 = std::make_shared<PointLight>("point_blue");
    pointLight2->setPosition(glm::vec3(3.0f, 2.0f, 0.0f));
    pointLight2->setColor(glm::vec3(0.3f, 0.3f, 1.0f));
    pointLight2->setIntensity(0.8f);
    pointLight2->setAttenuation(1.0f, 0.09f, 0.032f);
    lightManager.addLight(pointLight2);
    std::cout << "Added point light: point_blue" << std::endl;

    // Spotlight (flashlight-like, attached to camera position)
    auto spotlight = std::make_shared<SpotLight>("spotlight");
    spotlight->setPosition(camera.getPosition());
    spotlight->setDirection(camera.getFront());
    spotlight->setColor(glm::vec3(1.0f, 1.0f, 0.9f));
    spotlight->setIntensity(1.5f);
    spotlight->setCutoff(12.5f, 17.5f);
    spotlight->setAttenuation(1.0f, 0.09f, 0.032f);
    spotlight->setEnabled(false);  // Start disabled
    lightManager.addLight(spotlight);
    std::cout << "Added spotlight: spotlight (disabled by default)" << std::endl;

    std::cout << "Light system initialized with " << lightManager.getLightCount() << " lights" << std::endl;
}

void Application::initParticles() {
    // Initialize particle renderer
    particleRenderer_ = std::make_unique<ParticleRenderer>();
    if (!particleRenderer_->initialize()) {
        std::cerr << "Failed to initialize particle renderer" << std::endl;
        return;
    }

    // Initialize particle emitter with fire preset
    particleEmitter_ = std::make_unique<ParticleEmitter>(ParticlePresets::createFire());
    particleEmitter_->setPosition(glm::vec3(0.0f, 0.5f, 0.0f));

    std::cout << "Particle system initialized (Fire preset)" << std::endl;
}

void Application::initSkybox() {
    skybox_ = std::make_unique<Skybox>();
    skybox_->initialize();
    
    // Load default skybox (day)
    auto faces = SkyboxPresets::createDay();
    skybox_->loadCubemap(faces);
    
    std::cout << "Skybox initialized (Day preset)" << std::endl;
}

void Application::cycleSkyboxPreset() {
    if (!skybox_) return;
    
    skyboxPreset_ = (skyboxPreset_ + 1) % 4;
    
    switch (skyboxPreset_) {
        case 0:
            skybox_->loadCubemap(SkyboxPresets::createDay());
            std::cout << "Skybox: Day" << std::endl;
            break;
        case 1:
            skybox_->loadCubemap(SkyboxPresets::createNight());
            std::cout << "Skybox: Night" << std::endl;
            break;
        case 2:
            skybox_->loadCubemap(SkyboxPresets::createSunset());
            std::cout << "Skybox: Sunset" << std::endl;
            break;
        case 3:
            skybox_->loadCubemap(SkyboxPresets::createCloudy());
            std::cout << "Skybox: Cloudy" << std::endl;
            break;
    }
}

void Application::cycleParticlePreset() {
    if (!particleEmitter_) return;

    currentPreset_ = (currentPreset_ + 1) % 6;

    switch (currentPreset_) {
        case 0:
            particleEmitter_->setConfig(ParticlePresets::createFire());
            std::cout << "Particle preset: Fire" << std::endl;
            break;
        case 1:
            particleEmitter_->setConfig(ParticlePresets::createSmoke());
            std::cout << "Particle preset: Smoke" << std::endl;
            break;
        case 2:
            particleEmitter_->setConfig(ParticlePresets::createRain());
            std::cout << "Particle preset: Rain" << std::endl;
            break;
        case 3:
            particleEmitter_->setConfig(ParticlePresets::createSnow());
            std::cout << "Particle preset: Snow" << std::endl;
            break;
        case 4:
            particleEmitter_->setConfig(ParticlePresets::createExplosion());
            particleEmitter_->burst();
            std::cout << "Particle preset: Explosion (burst)" << std::endl;
            break;
        case 5:
            particleEmitter_->setConfig(ParticlePresets::createSparkle());
            std::cout << "Particle preset: Sparkle" << std::endl;
            break;
    }
}
