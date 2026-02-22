# REQ-001: 光照系统增强

## 需求描述
实现 Phong 光照模型，支持环境光、漫反射、镜面反射，并支持多个光源。

## 优先级
高

## 技术规格

### 1. 光源类型
- **方向光 (Directional Light)**：模拟太阳光，平行光
- **点光源 (Point Light)**：带衰减的点光源
- **聚光灯 (Spot Light)**：带角度限制的锥形光

### 2. 材质属性
```cpp
struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    float opacity;
    
    // 纹理
    std::shared_ptr<CTexture> diffuseMap;
    std::shared_ptr<CTexture> specularMap;
    std::shared_ptr<CTexture> normalMap;
};
```

### 3. 光照计算
- 环境光：`ambient * material.ambient`
- 漫反射：`max(dot(N, L), 0.0) * light.diffuse * material.diffuse`
- 镜面反射：`pow(max(dot(V, R), 0.0), material.shininess) * light.specular * material.specular`

### 4. 点光源衰减
```
attenuation = 1.0 / (constant + linear * distance + quadratic * distance^2)
```

## 文件结构
```
include/
  lighting/
    Light.h           # 光源基类和各种光源
    LightManager.h    # 光源管理器
  material/
    PBRMaterial.h     # PBR材质（可选扩展）
    
src/
  lighting/
    Light.cpp
    LightManager.cpp
  material/
    PBRMaterial.cpp

resources/shaders/
  lighting.vs         # 光照着色器顶点
  lighting.fs         # Phong 光照片段着色器
```

## 验收标准
1. ✅ 支持方向光、点光源、聚光灯
2. ✅ 支持材质的 ambient/diffuse/specular/shininess
3. ✅ 支持漫反射贴图和镜面贴图
4. ✅ 点光源支持距离衰减
5. ✅ 场景中至少支持 8 个动态光源
6. ✅ 单元测试覆盖率 100%
7. ✅ 接口文档完整

## 测试用例
- 测试光源位置、颜色、强度设置
- 测试材质属性设置
- 测试光照计算正确性
- 测试多光源叠加

## 估算工时
4-6 小时
