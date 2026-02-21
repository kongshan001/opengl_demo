# Application API 文档

## 概述

`Application` 类封装 OpenGL 应用程序的完整生命周期，包括窗口创建、初始化、渲染循环和资源管理。

## 头文件

```cpp
#include "core/Application.h"
```

## 配置结构

### AppConfig

```cpp
struct AppConfig {
    unsigned int width = 800;           // 窗口宽度
    unsigned int height = 600;          // 窗口高度
    std::string title = "OpenGL Demo";  // 窗口标题
    std::string shaderVertex;           // 顶点着色器路径
    std::string shaderFragment;         // 片段着色器路径
    std::string modelPath;              // 默认模型路径
    glm::vec3 backgroundColor;          // 背景色
};
```

## 构造函数

### Application()

```cpp
explicit Application(const AppConfig& config = AppConfig());
```

创建应用程序实例。使用 RAII，析构时自动清理资源。

## 生命周期方法

### initialize()

```cpp
bool initialize();
```

初始化应用程序，包括：
- GLFW 窗口创建
- GLAD OpenGL 加载器初始化
- OpenGL 状态设置
- 场景资源加载

**返回值**：
- `true` - 初始化成功
- `false` - 初始化失败

### run()

```cpp
void run();
```

运行主渲染循环。循环内自动处理：
- 帧时间计算
- 输入处理
- 场景渲染
- 缓冲交换

**注意**：此方法会阻塞直到窗口关闭。

### close()

```cpp
void close();
```

请求关闭应用程序。

### shouldClose()

```cpp
bool shouldClose() const;
```

检查窗口是否应该关闭。

## 属性访问

| 方法 | 返回类型 | 说明 |
|------|----------|------|
| `getWindow()` | GLFWwindow* | GLFW 窗口指针 |
| `getCamera()` | Camera& | 相机引用（可修改） |
| `getWidth()` | unsigned int | 窗口宽度 |
| `getHeight()` | unsigned int | 窗口高度 |

## 完整示例

### 基本用法

```cpp
#include "core/Application.h"

int main() {
    Application app;

    if (!app.initialize()) {
        return -1;
    }

    app.run();
    return 0;
}
```

### 自定义配置

```cpp
#include "core/Application.h"

int main() {
    AppConfig config;
    config.width = 1280;
    config.height = 720;
    config.title = "My OpenGL App";
    config.backgroundColor = glm::vec3(0.1f, 0.1f, 0.15f);
    config.modelPath = "resources/models/character.obj";

    Application app(config);

    if (!app.initialize()) {
        return -1;
    }

    app.run();
    return 0;
}
```

### 访问相机

```cpp
Application app;
app.initialize();

// 调整相机位置
app.getCamera().setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
app.getCamera().setMovementSpeed(5.0f);

app.run();
```

## 设计说明

- **RAII**：资源在构造时分配，析构时释放
- **不可拷贝**：禁止拷贝构造和赋值
- **窗口回调**：自动处理窗口大小变化
- **帧率无关**：使用 deltaTime 保证一致的运动速度
