#include "core/Frustum.h"

Frustum::Frustum() {
    // 初始化平面（默认为空）
}

Frustum::Plane Frustum::extractPlane(const glm::mat4& matrix, int row, int sign) {
    Plane plane;
    
    // 从视图投影矩阵中提取平面方程
    // 平面方程：Ax + By + Cz + D = 0
    // 其中 (A, B, C) 是法线，D 是距离
    
    plane.normal.x = matrix[0][3] + sign * matrix[0][row];
    plane.normal.y = matrix[1][3] + sign * matrix[1][row];
    plane.normal.z = matrix[2][3] + sign * matrix[2][row];
    plane.distance = matrix[3][3] + sign * matrix[3][row];
    
    // 归一化
    float length = glm::length(plane.normal);
    plane.normal /= length;
    plane.distance /= length;
    
    return plane;
}

void Frustum::update(const glm::mat4& viewProjectionMatrix) {
    // 从视图投影矩阵提取 6 个裁剪平面
    
    // 左平面（row 1, sign +1）
    planes[0] = extractPlane(viewProjectionMatrix, 1, 1);
    
    // 右平面（row 1, sign -1）
    planes[1] = extractPlane(viewProjectionMatrix, 1, -1);
    
    // 下平面（row 0, sign +1）
    planes[2] = extractPlane(viewProjectionMatrix, 0, 1);
    
    // 上平面（row 0, sign -1）
    planes[3] = extractPlane(viewProjectionMatrix, 0, -1);
    
    // 近平面（row 2, sign +1）
    planes[4] = extractPlane(viewProjectionMatrix, 2, 1);
    
    // 远平面（row 2, sign -1）
    planes[5] = extractPlane(viewProjectionMatrix, 2, -1);
}

bool Frustum::containsPoint(const glm::vec3& point) const {
    // 检查点是否在所有 6 个平面的内侧
    for (int i = 0; i < 6; i++) {
        if (planes[i].getDistance(point) < 0) {
            return false;  // 点在某个平面的外侧
        }
    }
    return true;  // 点在所有平面的内侧
}

bool Frustum::containsSphere(const glm::vec3& center, float radius) const {
    // 检查球体是否与所有 6 个平面相交或在内部
    for (int i = 0; i < 6; i++) {
        float distance = planes[i].getDistance(center);
        if (distance < -radius) {
            return false;  // 球体完全在某个平面的外侧
        }
    }
    return true;  // 球体在视锥体内或与平面相交
}

bool Frustum::containsBox(const glm::vec3& min, const glm::vec3& max) const {
    // AABB 包围盒的 8 个角点
    glm::vec3 corners[8] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(min.x, max.y, max.z),
        glm::vec3(max.x, max.y, max.z)
    };
    
    // 检查每个平面
    for (int i = 0; i < 6; i++) {
        bool allOutside = true;
        
        // 检查所有 8 个角点
        for (int j = 0; j < 8; j++) {
            if (planes[i].getDistance(corners[j]) >= 0) {
                allOutside = false;  // 至少有一个角点在平面内侧
                break;
            }
        }
        
        if (allOutside) {
            return false;  // 所有角点都在这个平面的外侧
        }
    }
    
    return true;  // 包围盒在视锥体内或与平面相交
}
