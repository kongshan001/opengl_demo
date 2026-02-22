#include "core/SceneSerializer.h"
#include <fstream>
#include <iostream>

// JSON 转换辅助函数
json SceneSerializer::vec3ToJson(const glm::vec3& v) {
    return {v.x, v.y, v.z};
}

glm::vec3 SceneSerializer::jsonToVec3(const json& j) {
    return glm::vec3(j[0], j[1], j[2]);
}

json SceneSerializer::lightToJson(const LightSource& light) {
    json j;
    j["position"] = vec3ToJson(light.position);
    j["color"] = vec3ToJson(light.color);
    j["orbitSpeed"] = light.orbitSpeed;
    j["orbitRadius"] = light.orbitRadius;
    j["orbitHeight"] = light.orbitHeight;
    j["orbitPhase"] = light.orbitPhase;
    j["animated"] = light.animated;
    return j;
}

LightSource SceneSerializer::jsonToLight(const json& j) {
    LightSource light;
    light.position = jsonToVec3(j["position"]);
    light.color = jsonToVec3(j["color"]);
    light.orbitSpeed = j["orbitSpeed"];
    light.orbitRadius = j["orbitRadius"];
    light.orbitHeight = j["orbitHeight"];
    light.orbitPhase = j["orbitPhase"];
    light.animated = j["animated"];
    return light;
}

bool SceneSerializer::saveScene(const SceneConfig& config, const std::string& filepath) {
    try {
        json j;
        
        // 光源
        j["lights"] = json::array();
        for (const auto& light : config.lights) {
            j["lights"].push_back(lightToJson(light));
        }
        j["lightAnimationEnabled"] = config.lightAnimationEnabled;
        
        // 显示
        j["displayMode"] = config.displayMode;
        j["wireframeMode"] = config.wireframeMode;
        j["isPaused"] = config.isPaused;
        
        // 背景
        j["backgroundColor"] = vec3ToJson(config.backgroundColor);
        
        // 摄像机
        j["camera"]["position"] = vec3ToJson(config.cameraPosition);
        j["camera"]["front"] = vec3ToJson(config.cameraFront);
        j["camera"]["up"] = vec3ToJson(config.cameraUp);
        j["camera"]["zoom"] = config.cameraZoom;
        
        // 写入文件
        std::ofstream file(filepath);
        file << j.dump(4);  // 缩进 4 格
        file.close();
        
        std::cout << "Scene saved to " << filepath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to save scene: " << e.what() << std::endl;
        return false;
    }
}

bool SceneSerializer::loadScene(SceneConfig& config, const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filepath << std::endl;
            return false;
        }
        
        json j;
        file >> j;
        file.close();
        
        // 加载光源
        config.lights.clear();
        if (j.contains("lights") && j["lights"].is_array()) {
            for (const auto& lightJson : j["lights"]) {
                config.lights.push_back(jsonToLight(lightJson));
            }
        }
        config.lightAnimationEnabled = j.value("lightAnimationEnabled", true);
        
        // 加载显示设置
        config.displayMode = j.value("displayMode", 0);
        config.wireframeMode = j.value("wireframeMode", false);
        config.isPaused = j.value("isPaused", false);
        
        // 加载背景
        if (j.contains("backgroundColor")) {
            config.backgroundColor = jsonToVec3(j["backgroundColor"]);
        }
        
        // 加载摄像机
        if (j.contains("camera")) {
            auto cameraJson = j["camera"];
            config.cameraPosition = jsonToVec3(cameraJson["position"]);
            config.cameraFront = jsonToVec3(cameraJson["front"]);
            config.cameraUp = jsonToVec3(cameraJson["up"]);
            config.cameraZoom = cameraJson["zoom"];
        }
        
        std::cout << "Scene loaded from " << filepath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load scene: " << e.what() << std::endl;
        return false;
    }
}
