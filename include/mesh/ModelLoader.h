#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include "mesh/Mesh.h"

/**
 * @brief Exception class for model loading errors.
 */
class ModelLoadException : public std::runtime_error {
public:
    explicit ModelLoadException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Model loader interface.
 * 
 * Abstract base class for format-specific model loaders.
 */
class IModelLoader {
public:
    virtual ~IModelLoader() = default;
    
    /**
     * @brief Loads a model from the specified file.
     * @param filepath Path to the model file.
     * @return Vector of meshes contained in the model.
     * @throws ModelLoadException If loading fails.
     */
    virtual std::vector<std::shared_ptr<CMesh>> loadModel(const std::string& filepath) = 0;
    
    /**
     * @brief Checks if this loader can handle the given file.
     * @param filepath Path to check.
     * @return true if the file extension is supported.
     */
    virtual bool canLoad(const std::string& filepath) const = 0;
    
    /**
     * @brief Returns the supported file extension.
     * @return Extension string (e.g., "obj").
     */
    virtual const char* getSupportedExtension() const = 0;
};

/**
 * @brief OBJ format model loader.
 */
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

/**
 * @brief Factory for creating model loaders.
 */
class ModelLoaderFactory {
public:
    /**
     * @brief Creates a loader for the given file.
     * @param filepath Path to the model file.
     * @return Unique pointer to a loader, or nullptr if unsupported.
     */
    static std::unique_ptr<IModelLoader> createLoader(const std::string& filepath);
    
    static void registerLoader(std::unique_ptr<IModelLoader> loader);
    
private:
    static std::vector<std::unique_ptr<IModelLoader>> loaders;
};

/**
 * @brief Main model loader class with simplified interface.
 */
class CModelLoader {
public:
    /**
     * @brief Loads a model file.
     * @param filepath Path to the model file.
     * @return Vector of meshes.
     * @throws ModelLoadException If loading fails or format is unsupported.
     */
    static std::vector<std::shared_ptr<CMesh>> load(const std::string& filepath);
    
    /**
     * @brief Checks if a file format is supported.
     * @param filepath Path to check.
     * @return true if supported.
     */
    static bool isSupported(const std::string& filepath);
    
    /**
     * @brief Returns list of supported formats.
     * @return Vector of supported extensions.
     */
    static std::vector<std::string> getSupportedFormats();

private:
    CModelLoader() = delete;
    ~CModelLoader() = delete;
};

#endif