# MeshUtils API 文档

## 概述

MeshUtils 提供几何体生成和网格处理的静态工具方法。

## 几何体生成

### createCube(float size = 1.0f)
创建一个立方体网格。

**参数**:
- `size`: 立方体边长（默认 1.0）

**返回**: `shared_ptr<CMesh>` 立方体网格

**示例**:
```cpp
auto cube = MeshUtils::createCube(2.0f);
```

---

### createSphere(float radius = 1.0f, unsigned int segments = 32)
创建一个 UV 球体网格。

**参数**:
- `radius`: 球体半径（默认 1.0）
- `segments`: 经纬度细分数量（默认 32）

**返回**: `shared_ptr<CMesh>` 球体网格

**顶点数**: (segments + 1)²

**示例**:
```cpp
auto sphere = MeshUtils::createSphere(1.0f, 64);  // 高精度球体
```

---

### createCylinder(float radius = 1.0f, float height = 1.0f, unsigned int segments = 32)
创建一个圆柱体网格。

**参数**:
- `radius`: 底面半径（默认 1.0）
- `height`: 圆柱高度（默认 1.0）
- `segments`: 圆周细分数量（默认 32）

**返回**: `shared_ptr<CMesh>` 圆柱体网格

**结构**:
- 侧面（平滑法线）
- 顶面圆盘
- 底面圆盘

**示例**:
```cpp
auto cylinder = MeshUtils::createCylinder(0.5f, 2.0f, 24);
```

---

### createCone(float radius = 1.0f, float height = 1.0f, unsigned int segments = 32)
创建一个圆锥体网格。

**参数**:
- `radius`: 底面半径（默认 1.0）
- `height`: 圆锥高度（默认 1.0）
- `segments`: 圆周细分数量（默认 32）

**返回**: `shared_ptr<CMesh>` 圆锥体网格

**示例**:
```cpp
auto cone = MeshUtils::createCone(1.0f, 2.0f, 16);
```

---

### createPlane(float width = 1.0f, float height = 1.0f, unsigned int widthSegments = 1, unsigned int heightSegments = 1)
创建一个平面网格（XZ 平面）。

**参数**:
- `width`: 宽度（默认 1.0）
- `height`: 深度（默认 1.0）
- `widthSegments`: 宽度细分（默认 1）
- `heightSegments`: 深度细分（默认 1）

**返回**: `shared_ptr<CMesh>` 平面网格

**示例**:
```cpp
auto ground = MeshUtils::createPlane(100.0f, 100.0f, 10, 10);  // 地面
```

---

## 网格处理

### calculateNormals(vertices, indices)
计算顶点法线（基于面法线平均）。

**参数**:
- `vertices`: 顶点数组
- `indices`: 索引数组

---

### calculateBoundingBox(vertices, bbox)
计算顶点的包围盒。

**参数**:
- `vertices`: 顶点数组
- `bbox`: 输出包围盒

---

### mergeMeshes(meshes)
合并多个网格为一个。

**参数**:
- `meshes`: 网格数组

**返回**: `shared_ptr<CMesh>` 合并后的网格

---

### createBoundingBoxVisualization(bbox)
创建包围盒线框可视化。

**参数**:
- `bbox`: 包围盒

**返回**: `shared_ptr<CMesh>` 线框网格（PrimitiveType::Lines）

---

*最后更新: 2026-02-21*
