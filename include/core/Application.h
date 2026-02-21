#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include "core/Camera.h"
#include "shader/Shader.h"
#include "mesh/Mesh.h"
#include "mesh/Material.h"

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
    glm::vec3 backgroundColor = glm::vec3(0.2f, 0.3f, 0.3f);
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
    std::shared_ptr<CMaterial> material;
    std::shared_ptr<CMesh> triangleMesh;
    std::vector<std::shared_ptr<CMesh>> modelMeshes;
    
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
};

#endif
