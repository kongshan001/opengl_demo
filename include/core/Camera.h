#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief 相机移动方向枚举
 */
enum class CameraMovement {
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down
};

/**
 * @brief 3D 相机类
 * 
 * 提供 FPS 风格的相机控制，支持 WASD 移动和鼠标视角旋转。
 * 自动计算视图矩阵和投影矩阵。
 * 
 * Example usage:
 * @code
 * Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
 * shader.setMat4("view", camera.getViewMatrix());
 * shader.setMat4("projection", camera.getProjectionMatrix(width, height));
 * @endcode
 */
class Camera {
public:
    /**
     * @brief 构造函数
     * @param position 相机初始位置
     * @param up 上方向向量
     * @param yaw 偏航角（度）
     * @param pitch 俯仰角（度）
     */
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float yaw = -90.0f,
                    float pitch = 0.0f);
    
    /**
     * @brief 获取视图矩阵
     * @return glm::mat4 视图矩阵
     */
    glm::mat4 getViewMatrix() const;
    
    /**
     * @brief 获取投影矩阵
     * @param width 视口宽度
     * @param height 视口高度
     * @param fov 视野角度（度），默认 45
     * @param nearPlane 近裁剪面，默认 0.1
     * @param farPlane 远裁剪面，默认 100.0
     * @return glm::mat4 投影矩阵
     */
    glm::mat4 getProjectionMatrix(float width, float height,
                                   float fov = 45.0f,
                                   float nearPlane = 0.1f,
                                   float farPlane = 100.0f) const;
    
    /**
     * @brief 处理键盘输入移动相机
     * @param direction 移动方向
     * @param deltaTime 帧间隔时间
     */
    void processKeyboard(CameraMovement direction, float deltaTime);
    
    /**
     * @brief 处理鼠标移动旋转视角
     * @param xoffset X 轴偏移量
     * @param yoffset Y 轴偏移量
     * @param constrainPitch 是否限制俯仰角
     */
    void processMouseMovement(float xoffset, float yoffset,
                              bool constrainPitch = true);
    
    /**
     * @brief 处理鼠标滚轮缩放
     * @param yoffset 滚轮偏移量
     */
    void processMouseScroll(float yoffset);
    
    // Getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }
    glm::vec3 getUp() const { return up; }
    glm::vec3 getRight() const { return right; }
    float getZoom() const { return zoom; }
    
    // Setters
    void setPosition(const glm::vec3& pos) { position = pos; }
    void setMovementSpeed(float speed) { movementSpeed = speed; }
    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }

private:
    // 相机属性
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    // 欧拉角
    float yaw;
    float pitch;
    
    // 相机选项
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    
    /**
     * @brief 根据欧拉角更新相机向量
     */
    void updateCameraVectors();
};

#endif
