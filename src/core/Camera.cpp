/**
 * @file Camera.cpp
 * @brief 3D Camera implementation with multiple modes
 */

#include "core/Camera.h"
#include <algorithm>  // for std::max, std::min
#include <cmath>

// Helper for C++11 compatibility (std::clamp requires C++17)
template<typename T>
inline T clampValue(T value, T minVal, T maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(2.5f),
      mouseSensitivity(0.1f),
      zoom(45.0f),
      mode_(CameraMode::FirstPerson) {
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    this->orbitYaw_ = yaw;
    this->orbitPitch_ = pitch;
    updateCameraVectors();
}

// ==================== Matrices ====================

glm::mat4 Camera::getViewMatrix() const {
    switch (mode_) {
        case CameraMode::ThirdPerson:
        case CameraMode::Orbit:
            return glm::lookAt(position, target_, up);
        case CameraMode::FirstPerson:
        case CameraMode::FreeFlight:
        default:
            return glm::lookAt(position, position + front, up);
    }
}

glm::mat4 Camera::getProjectionMatrix(float width, float height,
                                       float fov, float nearPlane, float farPlane) const {
    if (height == 0.0f) return glm::mat4(1.0f);
    return glm::perspective(glm::radians(fov), width / height, nearPlane, farPlane);
}

// ==================== Input Processing ====================

void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
    switch (mode_) {
        case CameraMode::FirstPerson:
            processKeyboardFirstPerson(direction, deltaTime);
            break;
        case CameraMode::ThirdPerson:
            processKeyboardThirdPerson(direction, deltaTime);
            break;
        case CameraMode::FreeFlight:
            processKeyboardFreeFlight(direction, deltaTime);
            break;
        case CameraMode::Orbit:
            // In orbit mode, WASD moves the target
            processKeyboardThirdPerson(direction, deltaTime);
            break;
    }
}

void Camera::processKeyboardFirstPerson(CameraMovement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    
    switch (direction) {
        case CameraMovement::Forward:
            position += front * velocity;
            break;
        case CameraMovement::Backward:
            position -= front * velocity;
            break;
        case CameraMovement::Left:
            position -= right * velocity;
            break;
        case CameraMovement::Right:
            position += right * velocity;
            break;
        case CameraMovement::Up:
            position += worldUp * velocity;
            break;
        case CameraMovement::Down:
            position -= worldUp * velocity;
            break;
    }
}

void Camera::processKeyboardThirdPerson(CameraMovement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    
    // Move the target, camera follows
    switch (direction) {
        case CameraMovement::Forward:
            target_ += glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * velocity;
            break;
        case CameraMovement::Backward:
            target_ -= glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * velocity;
            break;
        case CameraMovement::Left:
            target_ -= glm::normalize(glm::vec3(right.x, 0.0f, right.z)) * velocity;
            break;
        case CameraMovement::Right:
            target_ += glm::normalize(glm::vec3(right.x, 0.0f, right.z)) * velocity;
            break;
        case CameraMovement::Up:
            target_ += worldUp * velocity;
            break;
        case CameraMovement::Down:
            target_ -= worldUp * velocity;
            break;
    }
    
    updateOrbitPosition();
}

void Camera::processKeyboardFreeFlight(CameraMovement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    
    // 6DOF movement - movement follows camera orientation exactly
    switch (direction) {
        case CameraMovement::Forward:
            position += front * velocity;
            break;
        case CameraMovement::Backward:
            position -= front * velocity;
            break;
        case CameraMovement::Left:
            position -= right * velocity;
            break;
        case CameraMovement::Right:
            position += right * velocity;
            break;
        case CameraMovement::Up:
            position += up * velocity;  // Use local up instead of worldUp
            break;
        case CameraMovement::Down:
            position -= up * velocity;
            break;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    switch (mode_) {
        case CameraMode::FirstPerson:
        case CameraMode::FreeFlight:
            {
                xoffset *= mouseSensitivity;
                yoffset *= mouseSensitivity;
                
                yaw += xoffset;
                pitch += yoffset;
                
                if (constrainPitch) {
                    if (pitch > 89.0f) pitch = 89.0f;
                    if (pitch < -89.0f) pitch = -89.0f;
                }
                
                updateCameraVectors();
            }
            break;
        case CameraMode::ThirdPerson:
            processMouseThirdPerson(xoffset, yoffset);
            break;
        case CameraMode::Orbit:
            processMouseOrbit(xoffset, yoffset);
            break;
    }
}

void Camera::processMouseThirdPerson(float xoffset, float yoffset) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    
    orbitYaw_ += xoffset;
    orbitPitch_ += yoffset;
    
    // Constrain pitch
    if (orbitPitch_ > 89.0f) orbitPitch_ = 89.0f;
    if (orbitPitch_ < -89.0f) orbitPitch_ = -89.0f;
    
    updateOrbitPosition();
}

void Camera::processMouseOrbit(float xoffset, float yoffset) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    
    orbitYaw_ += xoffset;
    orbitPitch_ += yoffset;
    
    // Constrain pitch
    if (orbitPitch_ > 89.0f) orbitPitch_ = 89.0f;
    if (orbitPitch_ < -89.0f) orbitPitch_ = -89.0f;
    
    updateOrbitPosition();
}

void Camera::processMouseScroll(float yoffset) {
    switch (mode_) {
        case CameraMode::ThirdPerson:
        case CameraMode::Orbit:
            // Zoom by changing orbit distance
            orbitDistance_ -= yoffset * 0.5f;
            orbitDistance_ = std::max(1.0f, orbitDistance_);
            orbitDistance_ = std::min(50.0f, orbitDistance_);
            updateOrbitPosition();
            break;
        case CameraMode::FirstPerson:
        case CameraMode::FreeFlight:
        default:
            // Zoom by changing FOV
            zoom -= yoffset;
            if (zoom < 1.0f) zoom = 1.0f;
            if (zoom > 45.0f) zoom = 45.0f;
            break;
    }
}

// ==================== Mode Management ====================

void Camera::setMode(CameraMode mode) {
    CameraMode previousMode = mode_;
    mode_ = mode;
    
    // Initialize mode-specific settings
    switch (mode_) {
        case CameraMode::FirstPerson:
            // Reset to standard FPS behavior
            break;
        case CameraMode::ThirdPerson:
            // Set target to current look position
            if (previousMode == CameraMode::FirstPerson || previousMode == CameraMode::FreeFlight) {
                target_ = position + front * orbitDistance_;
                orbitYaw_ = yaw;
                orbitPitch_ = pitch;
            }
            updateOrbitPosition();
            break;
        case CameraMode::FreeFlight:
            // Keep current position and orientation
            break;
        case CameraMode::Orbit:
            // Center on scene origin
            if (previousMode == CameraMode::FirstPerson || previousMode == CameraMode::FreeFlight) {
                target_ = glm::vec3(0.0f);
                orbitYaw_ = yaw;
                orbitPitch_ = 20.0f;
            }
            updateOrbitPosition();
            break;
    }
}

void Camera::cycleMode() {
    int nextMode = (static_cast<int>(mode_) + 1) % 4;
    setMode(static_cast<CameraMode>(nextMode));
}

const char* Camera::getModeName() const {
    switch (mode_) {
        case CameraMode::FirstPerson:  return "First Person";
        case CameraMode::ThirdPerson:  return "Third Person";
        case CameraMode::FreeFlight:   return "Free Flight";
        case CameraMode::Orbit:        return "Orbit";
        default:                       return "Unknown";
    }
}

// ==================== Orbit Management ====================

void Camera::setOrbitDistance(float distance) {
    orbitDistance_ = std::max(1.0f, distance);
    if (mode_ == CameraMode::ThirdPerson || mode_ == CameraMode::Orbit) {
        updateOrbitPosition();
    }
}

void Camera::updateOrbitPosition() {
    // Calculate camera position from orbit angles and distance
    float yawRad = glm::radians(orbitYaw_);
    float pitchRad = glm::radians(orbitPitch_);
    
    position.x = target_.x + orbitDistance_ * cos(pitchRad) * sin(yawRad);
    position.y = target_.y + orbitDistance_ * sin(pitchRad);
    position.z = target_.z + orbitDistance_ * cos(pitchRad) * cos(yawRad);
    
    // Update camera vectors
    front = glm::normalize(target_ - position);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::update(float deltaTime) {
    // Auto-rotate in orbit mode
    if (mode_ == CameraMode::Orbit && autoRotate_) {
        orbitYaw_ += autoRotateSpeed_ * deltaTime;
        if (orbitYaw_ > 360.0f) orbitYaw_ -= 360.0f;
        updateOrbitPosition();
    }
}

// ==================== Private Methods ====================

void Camera::updateCameraVectors() {
    // Calculate new front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    
    // Recalculate right and up vectors
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}
