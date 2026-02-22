# API 参考：光照系统 (Lighting System)

## 概述

光照系统提供三种光源类型：方向光、点光源和聚光灯，支持 Phong 光照模型。

## 类图

```
Light (基类)
  ├── DirectionalLight (方向光)
  ├── PointLight (点光源)
  └── SpotLight (聚光灯)

LightManager (光源管理器)
```

---

## Light (基类)

### 枚举

```cpp
enum class LightType {
    Directional,    // 方向光
    Point,          // 点光源
    Spot            // 聚光灯
};
```

### 方法

| 方法 | 描述 |
|------|------|
| `setColor(const glm::vec3&)` | 设置光源颜色 |
| `setIntensity(float)` | 设置光照强度 |
| `setEnabled(bool)` | 启用/禁用光源 |
| `getColor()` | 获取颜色 |
| `getIntensity()` | 获取强度 |
| `isEnabled()` | 是否启用 |
| `getType()` | 获取光源类型 |
| `getName()` | 获取光源名称 |

---

## DirectionalLight

模拟太阳的平行光源。

### 构造函数

```cpp
DirectionalLight(const std::string& name = "");
```

### 方法

| 方法 | 描述 |
|------|------|
| `setDirection(const glm::vec3&)` | 设置光照方向（会被归一化） |
| `getDirection()` | 获取光照方向 |

### 示例

```cpp
auto sun = std::make_shared<DirectionalLight>("sun");
sun->setDirection(glm::vec3(0.5f, -1.0f, 0.3f));
sun->setColor(glm::vec3(1.0f, 0.95f, 0.9f));
sun->setIntensity(1.0f);
```

---

## PointLight

带距离衰减的点光源。

### 构造函数

```cpp
PointLight(const std::string& name = "");
```

### 方法

| 方法 | 描述 |
|------|------|
| `setPosition(const glm::vec3&)` | 设置位置 |
| `getPosition()` | 获取位置 |
| `setAttenuation(float c, float l, float q)` | 设置衰减参数 |
| `getConstant()` | 获取常数衰减 |
| `getLinear()` | 获取线性衰减 |
| `getQuadratic()` | 获取二次衰减 |

### 衰减公式

```
attenuation = 1.0 / (constant + linear * distance + quadratic * distance²)
```

### 预设衰减值

| 距离范围 | constant | linear | quadratic |
|----------|----------|--------|-----------|
| 7        | 1.0      | 0.7    | 1.8       |
| 13       | 1.0      | 0.35   | 0.44      |
| 20       | 1.0      | 0.22   | 0.20      |
| 32       | 1.0      | 0.14   | 0.07      |
| 50       | 1.0      | 0.09   | 0.032     |
| 100      | 1.0      | 0.045  | 0.0075    |

### 示例

```cpp
auto lamp = std::make_shared<PointLight>("lamp");
lamp->setPosition(glm::vec3(2.0f, 3.0f, 1.0f));
lamp->setColor(glm::vec3(1.0f, 0.8f, 0.6f));
lamp->setAttenuation(1.0f, 0.09f, 0.032f);  // 50 单位范围
```

---

## SpotLight

带角度限制的锥形光源。

### 构造函数

```cpp
SpotLight(const std::string& name = "");
```

### 方法

| 方法 | 描述 |
|------|------|
| `setPosition(const glm::vec3&)` | 设置位置 |
| `setDirection(const glm::vec3&)` | 设置朝向（会被归一化） |
| `setCutoff(float innerDeg, float outerDeg)` | 设置锥角（度） |
| `setAttenuation(float c, float l, float q)` | 设置衰减参数 |
| `getPosition()` | 获取位置 |
| `getDirection()` | 获取朝向 |
| `getInnerCutoff()` | 获取内锥角（cos值） |
| `getOuterCutoff()` | 获取外锥角（cos值） |

### 锥角说明

- **innerCutoff**: 内锥角，范围内光照最强
- **outerCutoff**: 外锥角，边缘处光照渐变到 0
- 值为 cosine 值，通过 `setCutoff(degrees)` 自动转换

### 示例

```cpp
auto flashlight = std::make_shared<SpotLight>("flashlight");
flashlight->setPosition(camera.getPosition());
flashlight->setDirection(camera.getFront());
flashlight->setCutoff(12.5f, 17.5f);  // 内12.5°，外17.5°
flashlight->setAttenuation(1.0f, 0.09f, 0.032f);
```

---

## LightManager

管理场景中的所有光源，最多支持 8 个动态光源。

### 常量

```cpp
static constexpr int MAX_LIGHTS = 8;
```

### 方法

| 方法 | 描述 |
|------|------|
| `addLight(shared_ptr<Light>)` | 添加光源 |
| `removeLight(const string&)` | 按名称移除光源 |
| `removeLight(size_t)` | 按索引移除光源 |
| `clearLights()` | 清除所有光源 |
| `getLight(const string&)` | 按名称获取光源 |
| `getLight(size_t)` | 按索引获取光源 |
| `getLightCount()` | 获取光源总数 |
| `getEnabledLightCount()` | 获取启用的光源数 |
| `getPointLights()` | 获取所有点光源 |
| `getDirectionalLights()` | 获取所有方向光 |
| `getSpotLights()` | 获取所有聚光灯 |
| `setAmbientColor(const glm::vec3&)` | 设置全局环境光 |
| `getAmbientColor()` | 获取全局环境光 |
| `forEachEnabledLight(callback)` | 遍历启用的光源 |

### 示例

```cpp
LightManager lightManager;
lightManager.setAmbientColor(glm::vec3(0.1f));

// 添加方向光
auto sun = std::make_shared<DirectionalLight>("sun");
sun->setDirection(glm::vec3(0.5f, -1.0f, 0.3f));
sun->setColor(glm::vec3(1.0f, 0.95f, 0.9f));
lightManager.addLight(sun);

// 添加点光源
auto point1 = std::make_shared<PointLight>("point1");
point1->setPosition(glm::vec3(2.0f, 2.0f, 0.0f));
point1->setColor(glm::vec3(1.0f, 0.5f, 0.0f));
lightManager.addLight(point1);

// 禁用某个光源
lightManager.getLight("point1")->setEnabled(false);

// 遍历启用的光源，设置着色器 uniform
lightManager.forEachEnabledLight([&shader](const Light* light, int index) {
    // 设置 shader uniforms...
});
```

---

## 着色器接口

### Uniform 变量

```glsl
// 材质
uniform struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

// 光源数据结构
struct LightData {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
    float innerCutoff;
    float outerCutoff;
    int type;       // 0=方向光, 1=点光源, 2=聚光灯
    int enabled;
};

uniform LightData lights[8];
uniform int numLights;
uniform vec3 ambientColor;
uniform vec3 viewPos;

// 纹理
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform bool hasDiffuseTexture;
uniform bool hasSpecularTexture;
```

### 使用着色器

```glsl
// 在片元着色器中
#include "phong.fs"
```

---

## 文件结构

```
include/lighting/
  Light.h           # 光源类定义
  LightManager.h    # 光源管理器

src/lighting/
  Light.cpp         # 光源类实现
  LightManager.cpp  # 光源管理器实现

resources/shaders/
  phong.vs          # Phong 顶点着色器
  phong.fs          # Phong 片元着色器
```
