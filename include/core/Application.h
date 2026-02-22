#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include "core/Camera.h"
#include "core/SceneSerializer.h"
#include "shader/Shader.h"
#include "mesh/Mesh.h"
#include "mesh/Material.h"
#include "mesh/Texture.h"

// ImGui 前向声明
struct ImGuiContext;

/**
 * @brief 应用程序配置结构
 */
struct AppConfig {
    unsigned int width = 800;
    unsigned int height = 600;
    std::string title = "OpenGL Demo";
    std::string shaderVertex = "resources/shaders/mesh.vs";
    std::string shaderFragment = "resources/shaders/mesh.fs";
    std::string modelPath = "resources/models/cube.obj";
    glm::vec3 backgroundColor = glm::vec3(0.3f, 0.35f, 0.4f);  // 浅灰蓝色背景
};

/**
 * @brief OpenGL 应用程序主类
 * 
 * 封装窗口创建、初始化、渲染循环和资源管理。
 * 使用 RAII 管理生命周期。
 * 
 * Example usage:
 * @code
 * Application app;
 * if (app.initialize()) {
 *     app.run();
 * }
 * @endcode
 */
class Application {
public:
    /**
     * @brief 构造函数
     * @param config 应用配置
     */
    explicit Application(const AppConfig& config = AppConfig());
    
    /**
     * @brief 析构函数，自动清理资源
     */
    ~Application();
    
    // 禁止拷贝
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    /**
     * @brief 初始化应用程序
     * @return true 初始化成功
     * @return false 初始化失败
     */
    bool initialize();
    
    /**
     * @brief 运行主循环
     */
    void run();
    
    /**
     * @brief 关闭应用程序
     */
    void close();
    
    /**
     * @brief 保存当前场景
     * @param filepath 文件路径
     * @return true 保存成功
     */
    bool saveScene(const std::string& filepath = "scene.json");
    
    /**
     * @brief 加载场景
     * @param filepath 文件路径
     * @return true 加载成功
     */
    bool loadScene(const std::string& filepath = "scene.json");
    
    /**
     * @brief 检查窗口是否应该关闭
     * @return true 窗口应该关闭
     */
    bool shouldClose() const;
    
    // Getters
    GLFWwindow* getWindow() const { return window; }
    Camera& getCamera() { return camera; }
    const Camera& getCamera() const { return camera; }
    unsigned int getWidth() const { return config.width; }
    unsigned int getHeight() const { return config.height; }

private:
    AppConfig config;
    GLFWwindow* window;
    Camera camera;
    
    // 资源
    std::shared_ptr<CShader> shader;
    std::shared_ptr<CShader> lightShader;  // 光源着色器
    std::shared_ptr<CMaterial> material;
    std::shared_ptr<CMesh> triangleMesh;
    std::shared_ptr<CMesh> texturedCube;
    std::shared_ptr<CMesh> sphereMesh;      // 球体
    std::shared_ptr<CMesh> cylinderMesh;    // 圆柱体
    std::shared_ptr<CMesh> coneMesh;        // 圆锥体
    std::shared_ptr<CMesh> lightIndicator;  // 光源指示器
    std::vector<std::shared_ptr<CMesh>> modelMeshes;
    
    // 纹理
    std::shared_ptr<CTexture> diffuseTexture;
    std::shared_ptr<CTexture> specularTexture;
    std::shared_ptr<CTexture> normalTexture;
    
    // 纹理参数
    bool textureWrapRepeat = true;   // 重复模式
    bool textureFilterLinear = true; // 线性过滤
    
    // 时间管理
    float deltaTime;
    float lastFrame;
    
    /**
     * @brief 初始化 GLFW 窗口
     * @return true 成功
     */
    bool initWindow();
    
    /**
     * @brief 初始化 GLAD 和 OpenGL 状态
     * @return true 成功
     */
    bool initOpenGL();
    
    /**
     * @brief 初始化 ImGui
     */
    void initImGui();
    
    /**
     * @brief 关闭 ImGui
     */
    void shutdownImGui();
    
    /**
     * @brief 初始化场景资源（着色器、材质、网格）
     */
    void initScene();
    
    /**
     * @brief 处理输入
     */
    void processInput();
    
    /**
     * @brief 更新帧时间
     */
    void updateDeltaTime();
    
    /**
     * @brief 渲染一帧
     */
    void render();
    
    /**
     * @brief 渲染 ImGui 界面
     */
    void renderImGui();
    
    /**
     * @brief 设置全局 uniform
     */
    void setGlobalUniforms();
    
    /**
     * @brief 渲染场景对象
     */
    void renderScene();
    
    /**
     * @brief GLFW 帧缓冲大小回调
     */
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    
    /**
     * @brief GLFW 鼠标移动回调
     */
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    
    /**
     * @brief GLFW 鼠标滚轮回调
     */
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
    // 鼠标控制状态
    bool firstMouse = true;
    float lastMouseX = 400.0f;
    float lastMouseY = 300.0f;
    
    // FPS 计算
    int frameCount = 0;
    float fpsTimer = 0.0f;
    float currentFPS = 0.0f;
    
    // 动画控制
    bool isPaused = false;
    float pausedTime = 0.0f;
    
    // 光源（使用 SceneSerializer 中定义的 LightSource）
    std::vector<LightSource> lights;
    bool lightAnimationEnabled = true;
    
    // 显示模式 (0=全部, 1=立方体, 2=球体, 3=圆柱体, 4=圆锥体)
    int displayMode = 0;
    bool wireframeMode = false;
    bool showHelp = false;  // F1 帮助显示
    bool showImGui = true;  // 显示 ImGui 界面
    bool showDemo = false;  // ImGui Demo 窗口
    
    // ImGui 相关
    bool showLightWindow = true;  // 光源控制窗口
    bool showStatsWindow = true;  // 性能统计窗口
    bool showTextureWindow = true; // 纹理控制窗口
};

#endif
