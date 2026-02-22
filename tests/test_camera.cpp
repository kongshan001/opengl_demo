/**
 * @file test_camera.cpp
 * @brief Unit tests for Camera class
 */

#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "core/Camera.h"

class CameraTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建默认相机
        camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    }
    
    void TearDown() override {
        delete camera;
    }
    
    Camera* camera;
};

// ============================================================================
// 构造函数测试
// ============================================================================

TEST_F(CameraTest, DefaultConstructor) {
    Camera cam;
    glm::vec3 pos = cam.getPosition();
    EXPECT_FLOAT_EQ(pos.x, 0.0f);
    EXPECT_FLOAT_EQ(pos.y, 0.0f);
    EXPECT_FLOAT_EQ(pos.z, 0.0f);
}

TEST_F(CameraTest, PositionConstructor) {
    Camera cam(glm::vec3(1.0f, 2.0f, 3.0f));
    glm::vec3 pos = cam.getPosition();
    EXPECT_FLOAT_EQ(pos.x, 1.0f);
    EXPECT_FLOAT_EQ(pos.y, 2.0f);
    EXPECT_FLOAT_EQ(pos.z, 3.0f);
}

TEST_F(CameraTest, FullConstructor) {
    Camera cam(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 30.0f);
    glm::vec3 pos = cam.getPosition();
    EXPECT_FLOAT_EQ(pos.x, 0.0f);
    EXPECT_FLOAT_EQ(pos.y, 0.0f);
    EXPECT_FLOAT_EQ(pos.z, 5.0f);
}

// ============================================================================
// 视图矩阵测试
// ============================================================================

TEST_F(CameraTest, GetViewMatrix) {
    glm::mat4 view = camera->getViewMatrix();
    
    // 视图矩阵应该是有效的
    // 检查矩阵不是零矩阵
    float sum = 0.0f;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            sum += std::abs(view[i][j]);
        }
    }
    EXPECT_GT(sum, 0.0f);
}

TEST_F(CameraTest, ViewMatrixLookAtOrigin) {
    Camera cam(glm::vec3(0.0f, 0.0f, 3.0f));
    glm::mat4 view = cam.getViewMatrix();
    
    // 与 glm::lookAt 比较应该近似相等
    glm::mat4 expected = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    // 验证视图矩阵的一些关键属性
    // 第3行第4列应该是相机位置的负Z分量
    EXPECT_NEAR(view[3][2], -3.0f, 0.1f);
}

// ============================================================================
// 投影矩阵测试
// ============================================================================

TEST_F(CameraTest, GetProjectionMatrix) {
    glm::mat4 proj = camera->getProjectionMatrix(800.0f, 600.0f);
    
    // 投影矩阵应该是有效的
    float sum = 0.0f;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            sum += std::abs(proj[i][j]);
        }
    }
    EXPECT_GT(sum, 0.0f);
}

TEST_F(CameraTest, ProjectionMatrixAspectRatio) {
    glm::mat4 proj1 = camera->getProjectionMatrix(800.0f, 600.0f);  // 4:3
    glm::mat4 proj2 = camera->getProjectionMatrix(1600.0f, 900.0f); // 16:9
    
    // 不同宽高比应该产生不同的投影矩阵
    bool different = false;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (std::abs(proj1[i][j] - proj2[i][j]) > 0.01f) {
                different = true;
                break;
            }
        }
    }
    EXPECT_TRUE(different);
}

TEST_F(CameraTest, ProjectionMatrixCustomFOV) {
    glm::mat4 proj45 = camera->getProjectionMatrix(800.0f, 600.0f, 45.0f);
    glm::mat4 proj90 = camera->getProjectionMatrix(800.0f, 600.0f, 90.0f);
    
    // 不同 FOV 应该产生不同的投影矩阵
    EXPECT_NE(proj45, proj90);
}

// ============================================================================
// 键盘输入测试
// ============================================================================

TEST_F(CameraTest, ProcessKeyboardForward) {
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Forward, 0.1f);
    glm::vec3 newPos = camera->getPosition();
    
    // 向前移动应该减少 Z 值（因为相机默认朝向 -Z）
    EXPECT_LT(newPos.z, initialPos.z);
}

TEST_F(CameraTest, ProcessKeyboardBackward) {
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Backward, 0.1f);
    glm::vec3 newPos = camera->getPosition();
    
    // 向后移动应该增加 Z 值
    EXPECT_GT(newPos.z, initialPos.z);
}

TEST_F(CameraTest, ProcessKeyboardLeft) {
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Left, 0.1f);
    glm::vec3 newPos = camera->getPosition();
    
    // 向左移动应该减少 X 值
    EXPECT_LT(newPos.x, initialPos.x);
}

TEST_F(CameraTest, ProcessKeyboardRight) {
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Right, 0.1f);
    glm::vec3 newPos = camera->getPosition();
    
    // 向右移动应该增加 X 值
    EXPECT_GT(newPos.x, initialPos.x);
}

TEST_F(CameraTest, ProcessKeyboardUp) {
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Up, 0.1f);
    glm::vec3 newPos = camera->getPosition();
    
    // 向上移动应该增加 Y 值
    EXPECT_GT(newPos.y, initialPos.y);
}

TEST_F(CameraTest, ProcessKeyboardDown) {
    camera->setPosition(glm::vec3(0.0f, 5.0f, 3.0f));
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Down, 0.1f);
    glm::vec3 newPos = camera->getPosition();
    
    // 向下移动应该减少 Y 值
    EXPECT_LT(newPos.y, initialPos.y);
}

TEST_F(CameraTest, ProcessKeyboardZeroDelta) {
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Forward, 0.0f);
    glm::vec3 newPos = camera->getPosition();
    
    // deltaTime 为 0 时不应该移动
    EXPECT_EQ(initialPos, newPos);
}

// ============================================================================
// 鼠标输入测试
// ============================================================================

TEST_F(CameraTest, ProcessMouseMovementPositiveX) {
    glm::vec3 initialFront = camera->getFront();
    camera->processMouseMovement(10.0f, 0.0f);
    glm::vec3 newFront = camera->getFront();
    
    // 正 X 偏移应该改变朝向
    EXPECT_NE(initialFront, newFront);
}

TEST_F(CameraTest, ProcessMouseMovementPositiveY) {
    glm::vec3 initialFront = camera->getFront();
    camera->processMouseMovement(0.0f, 10.0f);
    glm::vec3 newFront = camera->getFront();
    
    // 正 Y 偏移应该改变朝向
    EXPECT_NE(initialFront, newFront);
}

TEST_F(CameraTest, ProcessMouseMovementZeroOffset) {
    glm::vec3 initialFront = camera->getFront();
    camera->processMouseMovement(0.0f, 0.0f);
    glm::vec3 newFront = camera->getFront();
    
    // 零偏移不应该改变朝向
    EXPECT_EQ(initialFront, newFront);
}

TEST_F(CameraTest, ProcessMouseMovementConstrainPitch) {
    // 测试俯仰角限制
    camera->processMouseMovement(0.0f, 500.0f);  // 大的正值应该被限制到 89 度
    glm::vec3 front = camera->getFront();
    
    // Y 分量不应该超过 sin(89°) ≈ 0.9998
    EXPECT_LT(front.y, 1.0f);
    EXPECT_GT(front.y, -1.0f);
}

TEST_F(CameraTest, ProcessMouseMovementNegativePitch) {
    camera->processMouseMovement(0.0f, -500.0f);  // 大的负值应该被限制到 -89 度
    glm::vec3 front = camera->getFront();
    
    // Y 分量不应该低于 sin(-89°) ≈ -0.9998
    EXPECT_LT(front.y, 1.0f);
    EXPECT_GT(front.y, -1.0f);
}

// ============================================================================
// 滚轮测试
// ============================================================================

TEST_F(CameraTest, ProcessMouseScrollZoomIn) {
    float initialZoom = camera->getZoom();
    camera->processMouseScroll(1.0f);
    float newZoom = camera->getZoom();
    
    // 向上滚动应该减小 FOV（放大）
    EXPECT_LT(newZoom, initialZoom);
}

TEST_F(CameraTest, ProcessMouseScrollZoomOut) {
    // 先将 zoom 减小到一个非最大值
    camera->processMouseScroll(5.0f);  // 减小 zoom
    float initialZoom = camera->getZoom();
    camera->processMouseScroll(-1.0f);  // 增大 zoom（缩小）
    float newZoom = camera->getZoom();
    
    // 向下滚动应该增大 FOV（缩小）
    EXPECT_GT(newZoom, initialZoom);
}

TEST_F(CameraTest, ProcessMouseScrollZoomClampMin) {
    // 多次放大
    for (int i = 0; i < 100; i++) {
        camera->processMouseScroll(5.0f);
    }
    
    // FOV 不应该小于 1.0
    EXPECT_GE(camera->getZoom(), 1.0f);
}

TEST_F(CameraTest, ProcessMouseScrollZoomClampMax) {
    // 多次缩小
    for (int i = 0; i < 100; i++) {
        camera->processMouseScroll(-5.0f);
    }
    
    // FOV 不应该超过 45.0
    EXPECT_LE(camera->getZoom(), 45.0f);
}

// ============================================================================
// Getter/Setter 测试
// ============================================================================

TEST_F(CameraTest, SetPosition) {
    camera->setPosition(glm::vec3(10.0f, 20.0f, 30.0f));
    glm::vec3 pos = camera->getPosition();
    
    EXPECT_FLOAT_EQ(pos.x, 10.0f);
    EXPECT_FLOAT_EQ(pos.y, 20.0f);
    EXPECT_FLOAT_EQ(pos.z, 30.0f);
}

TEST_F(CameraTest, SetMovementSpeed) {
    camera->setMovementSpeed(5.0f);
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Forward, 1.0f);
    glm::vec3 newPos = camera->getPosition();
    
    // 速度 5.0，deltaTime 1.0，应该移动 5.0 单位
    float distance = glm::length(newPos - initialPos);
    EXPECT_FLOAT_EQ(distance, 5.0f);
}

TEST_F(CameraTest, SetMouseSensitivity) {
    camera->setMouseSensitivity(0.5f);
    glm::vec3 initialFront = camera->getFront();
    camera->processMouseMovement(10.0f, 0.0f);
    glm::vec3 newFront = camera->getFront();
    
    // 灵敏度改变后应该有不同的响应
    EXPECT_NE(initialFront, newFront);
}

TEST_F(CameraTest, GetUp) {
    glm::vec3 up = camera->getUp();
    // 默认上方向应该接近 (0, 1, 0)
    EXPECT_NEAR(up.y, 1.0f, 0.1f);
}

TEST_F(CameraTest, GetRight) {
    glm::vec3 right = camera->getRight();
    // 默认右方向应该接近 (1, 0, 0)
    EXPECT_NEAR(right.x, 1.0f, 0.1f);
}

// ============================================================================
// 边界测试
// ============================================================================

TEST_F(CameraTest, LargeMovementDelta) {
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Forward, 100.0f);
    glm::vec3 newPos = camera->getPosition();
    
    // 即使很大的 deltaTime 也应该正确计算
    EXPECT_LT(newPos.z, initialPos.z);
}

TEST_F(CameraTest, NegativeDelta) {
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Forward, -0.1f);
    glm::vec3 newPos = camera->getPosition();
    
    // 负的 deltaTime 会导致反向移动
    EXPECT_GT(newPos.z, initialPos.z);
}

TEST_F(CameraTest, ExtremePosition) {
    camera->setPosition(glm::vec3(10000.0f, 10000.0f, 10000.0f));
    glm::vec3 pos = camera->getPosition();
    
    EXPECT_FLOAT_EQ(pos.x, 10000.0f);
    EXPECT_FLOAT_EQ(pos.y, 10000.0f);
    EXPECT_FLOAT_EQ(pos.z, 10000.0f);
}

// ============================================================================
// Camera Mode Tests (REQ-001)
// ============================================================================

TEST_F(CameraTest, DefaultModeIsFirstPerson) {
    EXPECT_EQ(camera->getMode(), CameraMode::FirstPerson);
}

TEST_F(CameraTest, SetModeToThirdPerson) {
    camera->setMode(CameraMode::ThirdPerson);
    EXPECT_EQ(camera->getMode(), CameraMode::ThirdPerson);
}

TEST_F(CameraTest, SetModeToFreeFlight) {
    camera->setMode(CameraMode::FreeFlight);
    EXPECT_EQ(camera->getMode(), CameraMode::FreeFlight);
}

TEST_F(CameraTest, SetModeToOrbit) {
    camera->setMode(CameraMode::Orbit);
    EXPECT_EQ(camera->getMode(), CameraMode::Orbit);
}

TEST_F(CameraTest, CycleMode) {
    EXPECT_EQ(camera->getMode(), CameraMode::FirstPerson);
    
    camera->cycleMode();
    EXPECT_EQ(camera->getMode(), CameraMode::ThirdPerson);
    
    camera->cycleMode();
    EXPECT_EQ(camera->getMode(), CameraMode::FreeFlight);
    
    camera->cycleMode();
    EXPECT_EQ(camera->getMode(), CameraMode::Orbit);
    
    camera->cycleMode();
    EXPECT_EQ(camera->getMode(), CameraMode::FirstPerson);  // Cycles back
}

TEST_F(CameraTest, GetModeName) {
    EXPECT_STREQ(camera->getModeName(), "First Person");
    
    camera->setMode(CameraMode::ThirdPerson);
    EXPECT_STREQ(camera->getModeName(), "Third Person");
    
    camera->setMode(CameraMode::FreeFlight);
    EXPECT_STREQ(camera->getModeName(), "Free Flight");
    
    camera->setMode(CameraMode::Orbit);
    EXPECT_STREQ(camera->getModeName(), "Orbit");
}

TEST_F(CameraTest, ThirdPersonTargetAndDistance) {
    camera->setMode(CameraMode::ThirdPerson);
    camera->setTarget(glm::vec3(1.0f, 2.0f, 3.0f));
    camera->setOrbitDistance(10.0f);
    
    EXPECT_EQ(camera->getTarget(), glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_FLOAT_EQ(camera->getOrbitDistance(), 10.0f);
}

TEST_F(CameraTest, ThirdPersonOrbitsAroundTarget) {
    camera->setMode(CameraMode::ThirdPerson);
    camera->setTarget(glm::vec3(0.0f));
    camera->setOrbitDistance(5.0f);
    
    // Mouse movement should rotate around target
    glm::vec3 initialPos = camera->getPosition();
    camera->processMouseMovement(100.0f, 0.0f);
    glm::vec3 newPos = camera->getPosition();
    
    // Position should change
    EXPECT_NE(initialPos, newPos);
    
    // Distance to target should remain approximately the same
    float initialDist = glm::length(initialPos - camera->getTarget());
    float newDist = glm::length(newPos - camera->getTarget());
    EXPECT_NEAR(initialDist, newDist, 0.1f);
}

TEST_F(CameraTest, OrbitModeAutoRotate) {
    camera->setMode(CameraMode::Orbit);
    camera->setTarget(glm::vec3(0.0f));
    camera->setOrbitDistance(5.0f);
    camera->setAutoRotate(true);
    camera->setAutoRotateSpeed(90.0f);  // 90 degrees per second
    
    EXPECT_TRUE(camera->isAutoRotate());
    
    glm::vec3 initialPos = camera->getPosition();
    camera->update(1.0f);  // Update with 1 second
    glm::vec3 newPos = camera->getPosition();
    
    // Position should change due to auto-rotation
    EXPECT_NE(initialPos, newPos);
}

TEST_F(CameraTest, OrbitModeNoAutoRotate) {
    camera->setMode(CameraMode::Orbit);
    camera->setAutoRotate(false);
    
    EXPECT_FALSE(camera->isAutoRotate());
    
    glm::vec3 initialPos = camera->getPosition();
    camera->update(1.0f);
    glm::vec3 newPos = camera->getPosition();
    
    // Position should not change without auto-rotate
    EXPECT_EQ(initialPos, newPos);
}

TEST_F(CameraTest, FreeFlightMovementFollowsView) {
    camera->setMode(CameraMode::FreeFlight);
    camera->setPosition(glm::vec3(0.0f));
    camera->setYaw(0.0f);
    camera->setPitch(45.0f);  // Look up
    
    glm::vec3 initialPos = camera->getPosition();
    camera->processKeyboard(CameraMovement::Forward, 1.0f);
    glm::vec3 newPos = camera->getPosition();
    
    // In free flight, forward should move in view direction (upward)
    EXPECT_GT(newPos.y, initialPos.y);
}

TEST_F(CameraTest, ThirdPersonZoomChangesDistance) {
    camera->setMode(CameraMode::ThirdPerson);
    camera->setOrbitDistance(10.0f);
    
    camera->processMouseScroll(1.0f);  // Zoom in
    
    EXPECT_LT(camera->getOrbitDistance(), 10.0f);
}

TEST_F(CameraTest, ThirdPersonZoomClamped) {
    camera->setMode(CameraMode::ThirdPerson);
    camera->setOrbitDistance(5.0f);
    
    // Try to zoom too close
    for (int i = 0; i < 100; i++) {
        camera->processMouseScroll(10.0f);
    }
    EXPECT_GE(camera->getOrbitDistance(), 1.0f);
    
    // Try to zoom too far
    for (int i = 0; i < 100; i++) {
        camera->processMouseScroll(-10.0f);
    }
    EXPECT_LE(camera->getOrbitDistance(), 50.0f);
}

// main function is provided by the test framework
// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
