# Camera API 文档

## 概述

`Camera` 类提供 FPS 风格的 3D 相机控制，支持 WASD 键盘移动和鼠标视角旋转。

## 头文件

```cpp
#include "core/Camera.h"
```

## 构造函数

### Camera()

```cpp
explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                float yaw = -90.0f,
                float pitch = 0.0f);
```

创建相机实例。

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| position | glm::vec3 | (0,0,0) | 相机初始位置 |
| up | glm::vec3 | (0,1,0) | 世界上方向 |
| yaw | float | -90° | 偏航角 |
| pitch | float | 0° | 俯仰角 |

## 矩阵方法

### getViewMatrix()

```cpp
glm::mat4 getViewMatrix() const;
```

获取视图矩阵，用于着色器 uniform。

**返回值**：4x4 视图矩阵

**示例**：
```cpp
shader.setMat4("view", camera.getViewMatrix());
```

### getProjectionMatrix()

```cpp
glm::mat4 getProjectionMatrix(float width, float height,
                              float fov = 45.0f,
                              float nearPlane = 0.1f,
                              float farPlane = 100.0f) const;
```

获取透视投影矩阵。

| 参数 | 类型 | 说明 |
|------|------|------|
| width | float | 视口宽度 |
| height | float | 视口高度 |
| fov | float | 视野角度（度） |
| nearPlane | float | 近裁剪面距离 |
| farPlane | float | 远裁剪面距离 |

## 输入处理

### processKeyboard()

```cpp
void processKeyboard(CameraMovement direction, float deltaTime);
```

处理键盘移动。

| direction | 说明 |
|-----------|------|
| Forward | 向前（W） |
| Backward | 向后（S） |
| Left | 向左（A） |
| Right | 向右（D） |

**示例**：
```cpp
if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKeyboard(CameraMovement::Forward, deltaTime);
```

### processMouseMovement()

```cpp
void processMouseMovement(float xoffset, float yoffset,
                          bool constrainPitch = true);
```

处理鼠标移动旋转视角。俯仰角自动限制在 ±89° 以避免翻转。

### processMouseScroll()

```cpp
void processMouseScroll(float yoffset);
```

处理鼠标滚轮缩放（调整 FOV）。

## 属性访问

| 方法 | 返回类型 | 说明 |
|------|----------|------|
| `getPosition()` | glm::vec3 | 相机位置 |
| `getFront()` | glm::vec3 | 前方向向量 |
| `getUp()` | glm::vec3 | 上方向向量 |
| `getRight()` | glm::vec3 | 右方向向量 |
| `getZoom()` | float | 当前 FOV |
| `setPosition(vec3)` | void | 设置位置 |
| `setMovementSpeed(float)` | void | 设置移动速度 |
| `setMouseSensitivity(float)` | void | 设置鼠标灵敏度 |

## 完整示例

```cpp
// 创建相机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// 渲染循环中
while (!glfwWindowShouldClose(window)) {
    float deltaTime = currentFrame - lastFrame;

    // 处理输入
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Forward, deltaTime);

    // 设置着色器 uniform
    shader.use();
    shader.setMat4("view", camera.getViewMatrix());
    shader.setMat4("projection", camera.getProjectionMatrix(800, 600));
    shader.setVec3("viewPos", camera.getPosition());

    // ... 渲染场景
}
```
