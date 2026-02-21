# CTexture API 文档

## 概述

`CTexture` 类封装 OpenGL 纹理的加载和管理。

## 头文件

```cpp
#include "mesh/Texture.h"
```

## 纹理类型枚举

```cpp
enum class TextureType {
    Diffuse = 0,   // 漫反射贴图
    Specular,      // 高光贴图
    Normal,        // 法线贴图
    Height,        // 高度贴图
    Ambient,       // 环境光贴图
    Count
};
```

## 构造函数

### 从文件加载

```cpp
CTexture(const std::string& filepath, TextureType texType = TextureType::Diffuse);
```

| 参数 | 说明 |
|------|------|
| filepath | 纹理文件路径 |
| texType | 纹理类型 |

**支持格式**：JPG, PNG, BMP, TGA, PSD, GIF, HDR, PIC

**示例**：
```cpp
auto texture = std::make_shared<CTexture>(
    "resources/textures/container.png",
    TextureType::Diffuse
);
```

### 从数据创建

```cpp
CTexture(unsigned char* data, int width, int height, int channels, TextureType texType = TextureType::Diffuse);
```

## 绑定方法

### bind()

```cpp
void bind(unsigned int textureUnit = 0) const;
```

绑定纹理到指定纹理单元。

```cpp
texture->bind(0);  // 绑定到 GL_TEXTURE0
```

### unbind()

```cpp
static void unbind(unsigned int textureUnit = 0);
```

## 纹理参数

### setWrapMode()

```cpp
void setWrapMode(GLenum wrapS, GLenum wrapT);
```

设置纹理包装模式。常用值：`GL_REPEAT`, `GL_MIRRORED_REPEAT`, `GL_CLAMP_TO_EDGE`。

### setFilterMode()

```cpp
void setFilterMode(GLenum minFilter, GLenum magFilter);
```

设置纹理过滤模式。常用值：`GL_LINEAR`, `GL_NEAREST`, `GL_LINEAR_MIPMAP_LINEAR`。

### generateMipmaps()

```cpp
void generateMipmaps();
```

生成多级渐远纹理。

## 属性访问

| 属性 | 类型 | 说明 |
|------|------|------|
| ID | unsigned int | OpenGL 纹理 ID |
| type | TextureType | 纹理类型 |
| path | std::string | 文件路径 |
| width | int | 纹理宽度 |
| height | int | 纹理高度 |
| nrChannels | int | 颜色通道数 |

## 完整示例

```cpp
#include "mesh/Texture.h"

// 加载纹理
auto diffuseTexture = std::make_shared<CTexture>(
    "resources/textures/container.png",
    TextureType::Diffuse
);

std::cout << "Texture size: " 
          << diffuseTexture->width << "x" << diffuseTexture->height 
          << std::endl;

// 在渲染循环中使用
shader.use();
glActiveTexture(GL_TEXTURE0);
diffuseTexture->bind(0);
shader.setInt("diffuseTexture", 0);

// 绘制网格
mesh->draw();
```
