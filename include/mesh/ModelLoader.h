#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include <vector>
#include <memory>
#include "mesh/Mesh.h"

// 模型加载器接口
class IModelLoader {
public:
    virtual ~IModelLoader() = default;
    virtual std::vector<std::shared_ptr<CMesh>> loadModel(const std::string& filepath) = 0;
    virtual bool canLoad(const std::string& filepath) const = 0;
    virtual const char* getSupportedExtension() const = 0;
};

// OBJ格式加载器
class OBJLoader : public IModelLoader {
public:
    std::vector<std::shared_ptr<CMesh>> loadModel(const std::string& filepath) override;
    bool canLoad(const std::string& filepath) const override;
    const char* getSupportedExtension() const override { return "obj"; }

private:
    struct OBJIndex {
        unsigned int positionIndex;
        unsigned int normalIndex;
        unsigned int texCoordIndex;
        
        bool operator<(const OBJIndex& other) const {
            return std::tie(positionIndex, normalIndex, texCoordIndex) < 
                   std::tie(other.positionIndex, other.normalIndex, other.texCoordIndex);
        }
    };
    
    void parseOBJ(const std::string& content, 
                  std::vector<glm::vec3>& positions,
                  std::vector<glm::vec3>& normals,
                  std::vector<glm::vec2>& texCoords,
                  std::vector<OBJIndex>& indices);
                  
    void createMesh(const std::vector<glm::vec3>& positions,
                   const std::vector<glm::vec3>& normals,
                   const std::vector<glm::vec2>& texCoords,
                   const std::vector<OBJIndex>& indices,
                   std::vector<std::shared_ptr<CMesh>>& meshes);
};

// 模型加载器工厂
class ModelLoaderFactory {
public:
    static IModelLoader* createLoader(const std::string& filepath);
    static void registerLoader(std::unique_ptr<IModelLoader> loader);
    
private:
    static std::vector<std::unique_ptr<IModelLoader>> loaders;
};

// 主模型加载器类（简化接口）
class CModelLoader {
public:
    // 加载模型文件
    static std::vector<std::shared_ptr<CMesh>> load(const std::string& filepath);
    
    // 检查文件格式是否支持
    static bool isSupported(const std::string& filepath);
    
    // 获取支持的文件格式列表
    static std::vector<std::string> getSupportedFormats();

private:
    CModelLoader() = default;
};

#endif