#ifndef SCENE_SERIALIZER_H
#define SCENE_SERIALIZER_H

#include <string>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include "core/Camera.h"

using json = nlohmann::json;

// 光源结构
struct LightSource {
    glm::vec3 position;
    glm::vec3 color;
    float orbitSpeed;
    float orbitRadius;
    float orbitHeight;
    float orbitPhase;
    bool animated;
};

// 场景配置
struct SceneConfig {
    // 光源
    std::vector<LightSource> lights;
    bool lightAnimationEnabled;
    
    // 显示
    int displayMode;
    bool wireframeMode;
    bool isPaused;
    
    // 背景
    glm::vec3 backgroundColor;
    
    // 摄像机
    glm::vec3 cameraPosition;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float cameraZoom;
};

class SceneSerializer {
public:
    /**
     * @brief 保存场景到 JSON 文件
     * @param config 场景配置
     * @param filepath 文件路径
     * @return true 保存成功
     */
    static bool saveScene(const SceneConfig& config, const std::string& filepath);
    
    /**
     * @brief 从 JSON 文件加载场景
     * @param config 输出的场景配置
     * @param filepath 文件路径
     * @return true 加载成功
     */
    static bool loadScene(SceneConfig& config, const std::string& filepath);
    
private:
    // JSON 转换辅助函数
    static json vec3ToJson(const glm::vec3& v);
    static glm::vec3 jsonToVec3(const json& j);
    static json lightToJson(const LightSource& light);
    static LightSource jsonToLight(const json& j);
};

#endif