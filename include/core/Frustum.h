#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm/glm.hpp>

/**
 * @brief 视锥体剔除类
 * 
 * 用于剔除不在摄像机视野内的物体，提升渲染性能。
 */
class Frustum {
public:
    /**
     * @brief 平面结构
     */
    struct Plane {
        glm::vec3 normal;  // 平面法线（指向视锥体内部）
        float distance;   // 原点到平面的距离

        /**
         * @brief 计算点到平面的距离
         * @param point 测试点
         * @return 距离（正数表示在平面内侧，负数表示在外侧）
         */
        float getDistance(const glm::vec3& point) const {
            return glm::dot(normal, point) + distance;
        }
    };

    /**
     * @brief 构造函数
     */
    Frustum();

    /**
     * @brief 从视图投影矩阵更新视锥体
     * @param viewProjectionMatrix 视图投影矩阵
     */
    void update(const glm::mat4& viewProjectionMatrix);

    /**
     * @brief 检查点是否在视锥体内
     * @param point 测试点
     * @return true 在视锥体内
     */
    bool containsPoint(const glm::vec3& point) const;

    /**
     * @brief 检查球体是否在视锥体内
     * @param center 球心
     * @param radius 半径
     * @return true 在视锥体内
     */
    bool containsSphere(const glm::vec3& center, float radius) const;

    /**
     * @brief 检查 AABB 包围盒是否在视锥体内
     * @param min 最小点
     * @param max 最大点
     * @return true 在视锥体内或相交
     */
    bool containsBox(const glm::vec3& min, const glm::vec3& max) const;

private:
    Plane planes[6];  // 6 个裁剪平面：左、右、上、下、近、远

    /**
     * @brief 从矩阵的行提取平面
     * @param matrix 矩阵
     * @param row 行索引
     * @param sign 符号（1 或 -1）
     * @return 提取的平面
     */
    Plane extractPlane(const glm::mat4& matrix, int row, int sign);
};

#endif