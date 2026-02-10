#ifndef VERTEX_H
#define VERTEX_H

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    
    // 默认构造函数
    Vertex() : 
        position(0.0f),
        normal(0.0f),
        texCoords(0.0f),
        tangent(0.0f),
        bitangent(0.0f) {}
    
    // 便利构造函数
    Vertex(const glm::vec3& pos) :
        position(pos),
        normal(0.0f),
        texCoords(0.0f),
        tangent(0.0f),
        bitangent(0.0f) {}
        
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex) :
        position(pos),
        normal(norm),
        texCoords(tex),
        tangent(0.0f),
        bitangent(0.0f) {}
        
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex, 
           const glm::vec3& tan, const glm::vec3& bitan) :
        position(pos),
        normal(norm),
        texCoords(tex),
        tangent(tan),
        bitangent(bitan) {}
};

enum class VertexAttribute {
    Position = 0,
    Normal,
    TexCoords,
    Tangent,
    Bitangent,
    Count
};

struct VertexAttributeLayout {
    struct Attribute {
        VertexAttribute type;
        unsigned int count;
        unsigned int offset;
        
        Attribute(VertexAttribute t, unsigned int c, unsigned int o) 
            : type(t), count(c), offset(o) {}
    };
    
    std::vector<Attribute> attributes;
    unsigned int stride;
    
    VertexAttributeLayout() : stride(0) {}
    
    void addAttribute(VertexAttribute type, unsigned int count) {
        attributes.emplace_back(type, count, stride);
        stride += count * sizeof(float);
    }
    
    // 常用布局预设
    static VertexAttributeLayout PositionOnly() {
        VertexAttributeLayout layout;
        layout.addAttribute(VertexAttribute::Position, 3);
        return layout;
    }
    
    static VertexAttributeLayout PositionNormal() {
        VertexAttributeLayout layout;
        layout.addAttribute(VertexAttribute::Position, 3);
        layout.addAttribute(VertexAttribute::Normal, 3);
        return layout;
    }
    
    static VertexAttributeLayout PositionNormalTex() {
        VertexAttributeLayout layout;
        layout.addAttribute(VertexAttribute::Position, 3);
        layout.addAttribute(VertexAttribute::Normal, 3);
        layout.addAttribute(VertexAttribute::TexCoords, 2);
        return layout;
    }
    
    static VertexAttributeLayout Full() {
        VertexAttributeLayout layout;
        layout.addAttribute(VertexAttribute::Position, 3);
        layout.addAttribute(VertexAttribute::Normal, 3);
        layout.addAttribute(VertexAttribute::TexCoords, 2);
        layout.addAttribute(VertexAttribute::Tangent, 3);
        layout.addAttribute(VertexAttribute::Bitangent, 3);
        return layout;
    }
};

#endif