/**
 * @file Camera.h
 * @brief 3D Camera with multiple modes (First Person, Third Person, Free Flight, Orbit)
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

/**
 * @brief Camera movement direction enumeration
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
 * @brief Camera mode enumeration
 */
enum class CameraMode {
    FirstPerson,    // FPS style - WASD + mouse look
    ThirdPerson,    // Orbit around target
    FreeFlight,     // 6DOF free movement
    Orbit           // Auto-rotate around scene center
};

/**
 * @brief 3D Camera Class
 * 
 * Provides multiple camera modes for different control styles.
 * Supports WASD movement, mouse look, and smooth mode transitions.
 * 
 * Example usage:
 * @code
 * Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
 * camera.setMode(CameraMode::ThirdPerson);
 * camera.setTarget(glm::vec3(0.0f));
 * shader.setMat4("view", camera.getViewMatrix());
 * @endcode
 */
class Camera {
public:
    /**
     * @brief Constructor
     * @param position Camera initial position
     * @param up Up direction vector
     * @param yaw Yaw angle (degrees)
     * @param pitch Pitch angle (degrees)
     */
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float yaw = -90.0f,
                    float pitch = 0.0f);
    
    // ==================== Matrices ====================
    
    /**
     * @brief Get view matrix
     * @return glm::mat4 View matrix
     */
    glm::mat4 getViewMatrix() const;
    
    /**
     * @brief Get projection matrix
     * @param width Viewport width
     * @param height Viewport height
     * @param fov Field of view (degrees), default 45
     * @param nearPlane Near clipping plane, default 0.1
     * @param farPlane Far clipping plane, default 100.0
     * @return glm::mat4 Projection matrix
     */
    glm::mat4 getProjectionMatrix(float width, float height,
                                   float fov = 45.0f,
                                   float nearPlane = 0.1f,
                                   float farPlane = 100.0f) const;
    
    // ==================== Input Processing ====================
    
    /**
     * @brief Process keyboard input to move camera
     * @param direction Movement direction
     * @param deltaTime Frame delta time
     */
    void processKeyboard(CameraMovement direction, float deltaTime);
    
    /**
     * @brief Process mouse movement to rotate view
     * @param xoffset X axis offset
     * @param yoffset Y axis offset
     * @param constrainPitch Whether to constrain pitch angle
     */
    void processMouseMovement(float xoffset, float yoffset,
                              bool constrainPitch = true);
    
    /**
     * @brief Process mouse scroll for zoom
     * @param yoffset Scroll offset
     */
    void processMouseScroll(float yoffset);
    
    // ==================== Mode Management ====================
    
    /**
     * @brief Set camera mode
     * @param mode New camera mode
     */
    void setMode(CameraMode mode);
    
    /**
     * @brief Get current camera mode
     * @return Current CameraMode
     */
    CameraMode getMode() const { return mode_; }
    
    /**
     * @brief Cycle to next camera mode
     */
    void cycleMode();
    
    /**
     * @brief Get mode name as string
     * @return Mode name string
     */
    const char* getModeName() const;
    
    // ==================== Target Management ====================
    
    /**
     * @brief Set target position for third person/orbit mode
     * @param target Target position
     */
    void setTarget(const glm::vec3& target) { target_ = target; }
    
    /**
     * @brief Get target position
     * @return Target position
     */
    glm::vec3 getTarget() const { return target_; }
    
    /**
     * @brief Set orbit distance for third person/orbit mode
     * @param distance Distance from target
     */
    void setOrbitDistance(float distance);
    
    /**
     * @brief Get orbit distance
     * @return Orbit distance
     */
    float getOrbitDistance() const { return orbitDistance_; }
    
    // ==================== Getters ====================
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }
    glm::vec3 getUp() const { return up; }
    glm::vec3 getRight() const { return right; }
    float getZoom() const { return zoom; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }
    
    // ==================== Setters ====================
    
    void setPosition(const glm::vec3& pos) { position = pos; }
    void setMovementSpeed(float speed) { movementSpeed = speed; }
    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void setYaw(float y) { yaw = y; updateCameraVectors(); }
    void setPitch(float p) { pitch = p; updateCameraVectors(); }
    
    // ==================== Orbit Options ====================
    
    /**
     * @brief Enable/disable auto-rotation in orbit mode
     * @param enable Enable auto-rotation
     */
    void setAutoRotate(bool enable) { autoRotate_ = enable; }
    
    /**
     * @brief Check if auto-rotation is enabled
     * @return True if auto-rotation is enabled
     */
    bool isAutoRotate() const { return autoRotate_; }
    
    /**
     * @brief Set auto-rotation speed
     * @param speed Rotation speed (degrees per second)
     */
    void setAutoRotateSpeed(float speed) { autoRotateSpeed_ = speed; }
    
    /**
     * @brief Update camera (call each frame for auto-rotate, etc.)
     * @param deltaTime Frame delta time
     */
    void update(float deltaTime);

private:
    // Camera attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    // Euler angles
    float yaw;
    float pitch;
    
    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    
    // Camera mode
    CameraMode mode_ = CameraMode::FirstPerson;
    
    // Third person / Orbit mode
    glm::vec3 target_ = glm::vec3(0.0f);
    float orbitDistance_ = 5.0f;
    float orbitYaw_ = 0.0f;
    float orbitPitch_ = 20.0f;
    
    // Orbit options
    bool autoRotate_ = false;
    float autoRotateSpeed_ = 30.0f;  // degrees per second
    
    /**
     * @brief Update camera vectors based on Euler angles
     */
    void updateCameraVectors();
    
    /**
     * @brief Update position for third person/orbit mode
     */
    void updateOrbitPosition();
    
    /**
     * @brief Process keyboard for first person mode
     */
    void processKeyboardFirstPerson(CameraMovement direction, float deltaTime);
    
    /**
     * @brief Process keyboard for third person mode
     */
    void processKeyboardThirdPerson(CameraMovement direction, float deltaTime);
    
    /**
     * @brief Process keyboard for free flight mode
     */
    void processKeyboardFreeFlight(CameraMovement direction, float deltaTime);
    
    /**
     * @brief Process mouse for third person mode
     */
    void processMouseThirdPerson(float xoffset, float yoffset);
    
    /**
     * @brief Process mouse for orbit mode
     */
    void processMouseOrbit(float xoffset, float yoffset);
};

#endif
