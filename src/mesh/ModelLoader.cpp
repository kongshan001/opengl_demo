#include "mesh/ModelLoader.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

// OBJLoader实现
std::vector<std::shared_ptr<CMesh>> OBJLoader::loadModel(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ModelLoadException("Failed to open OBJ file: " + filepath);
    }
    
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<OBJIndex> indices;
    
    std::string line;
    while (std::getline(file, line)) {
        parseOBJ(line, positions, normals, texCoords, indices);
    }
    
    file.close();
    
    if (positions.empty()) {
        throw ModelLoadException("OBJ file contains no vertices: " + filepath);
    }
    
    std::vector<std::shared_ptr<CMesh>> meshes;
    createMesh(positions, normals, texCoords, indices, meshes);
    
    return meshes;
}

bool OBJLoader::canLoad(const std::string& filepath) const {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) return false;
    
    std::string ext = filepath.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return ext == "obj";
}

void OBJLoader::parseOBJ(const std::string& content, 
                        std::vector<glm::vec3>& positions,
                        std::vector<glm::vec3>& normals,
                        std::vector<glm::vec2>& texCoords,
                        std::vector<OBJIndex>& indices) {
    std::stringstream ss(content);
    std::string type;
    ss >> type;
    
    if (type == "v") {
        glm::vec3 position;
        ss >> position.x >> position.y >> position.z;
        positions.push_back(position);
    } else if (type == "vn") {
        glm::vec3 normal;
        ss >> normal.x >> normal.y >> normal.z;
        normals.push_back(normal);
    } else if (type == "vt") {
        glm::vec2 texCoord;
        ss >> texCoord.x >> texCoord.y;
        texCoords.push_back(texCoord);
    } else if (type == "f") {
        std::string faceData;
        while (ss >> faceData) {
            OBJIndex index;
            size_t slash1 = faceData.find('/');
            size_t slash2 = faceData.find('/', slash1 + 1);
            
            // 位置索引 (OBJ 使用 1-based 索引)
            int posIdx = std::stoi(faceData.substr(0, slash1));
            index.positionIndex = static_cast<unsigned int>(posIdx > 0 ? posIdx - 1 : positions.size() + posIdx);
            
            if (slash2 != std::string::npos && slash2 != slash1 + 1) {
                // 纹理坐标索引
                int texIdx = std::stoi(faceData.substr(slash1 + 1, slash2 - slash1 - 1));
                index.texCoordIndex = static_cast<unsigned int>(texIdx > 0 ? texIdx - 1 : texCoords.size() + texIdx);
            } else {
                index.texCoordIndex = 0;
            }
            
            if (slash2 != std::string::npos && slash2 + 1 < faceData.length()) {
                // 法线索引
                int normIdx = std::stoi(faceData.substr(slash2 + 1));
                index.normalIndex = static_cast<unsigned int>(normIdx > 0 ? normIdx - 1 : normals.size() + normIdx);
            } else {
                index.normalIndex = 0;
            }
            
            indices.push_back(index);
        }
    }
}

void OBJLoader::createMesh(const std::vector<glm::vec3>& positions,
                         const std::vector<glm::vec3>& normals,
                         const std::vector<glm::vec2>& texCoords,
                         const std::vector<OBJIndex>& indices,
                         std::vector<std::shared_ptr<CMesh>>& meshes) {
    if (indices.empty()) return;
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> meshIndices;
    
    // 索引化顶点，消除重复
    std::unordered_map<size_t, unsigned int> vertexMap;
    
    for (const auto& idx : indices) {
        // 边界检查
        if (idx.positionIndex >= positions.size()) {
            throw ModelLoadException("Invalid position index in OBJ file");
        }
        
        // 创建哈希键
        size_t key = std::hash<std::string>{}(
            std::to_string(idx.positionIndex) + "|" +
            std::to_string(idx.normalIndex) + "|" +
            std::to_string(idx.texCoordIndex)
        );
        
        auto it = vertexMap.find(key);
        if (it != vertexMap.end()) {
            meshIndices.push_back(it->second);
        } else {
            Vertex vertex;
            
            // 位置（已验证边界）
            vertex.position = positions[idx.positionIndex];
            
            // 法线（可选）
            if (!normals.empty() && idx.normalIndex < normals.size()) {
                vertex.normal = normals[idx.normalIndex];
            }
            
            // 纹理坐标（可选）
            if (!texCoords.empty() && idx.texCoordIndex < texCoords.size()) {
                vertex.texCoords = texCoords[idx.texCoordIndex];
            }
            
            vertices.push_back(vertex);
            unsigned int vertexIndex = static_cast<unsigned int>(vertices.size() - 1);
            vertexMap[key] = vertexIndex;
            meshIndices.push_back(vertexIndex);
        }
    }
    
    // 创建网格
    auto mesh = std::shared_ptr<CMesh>(new CMesh(vertices, meshIndices));
    mesh->calculateNormals(); // 如果没有法线，计算默认法线
    mesh->calculateBoundingBox();
    
    meshes.push_back(mesh);
}

std::vector<std::unique_ptr<IModelLoader>> ModelLoaderFactory::loaders;

std::unique_ptr<IModelLoader> ModelLoaderFactory::createLoader(const std::string& filepath) {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return nullptr;
    }
    
    std::string extension = filepath.substr(dotPos + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "obj") {
        return std::unique_ptr<IModelLoader>(new OBJLoader());
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<CMesh>> CModelLoader::load(const std::string& filepath) {
    auto loader = ModelLoaderFactory::createLoader(filepath);
    if (!loader) {
        throw ModelLoadException("Unsupported model format: " + filepath);
    }
    
    return loader->loadModel(filepath);
}

bool CModelLoader::isSupported(const std::string& filepath) {
    auto loader = ModelLoaderFactory::createLoader(filepath);
    return loader != nullptr;
}

std::vector<std::string> CModelLoader::getSupportedFormats() {
    return {"obj"};
}
