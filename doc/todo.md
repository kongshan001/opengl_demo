# TODO 列表

本文档记录项目未来的开发计划和待完成的任务。

## 优先级分类

- 🔴 高优先级 - 核心功能，近期完成
- 🟡 中优先级 - 重要功能，计划完成
- 🟢 低优先级 - 增强功能，有时间再做

## 短期计划 (1-2 周)

### 后期处理 (REQ-025) 🔴

- [ ] 帧缓冲（FBO）
- [ ] 多重渲染目标（MRT）
- [ ] 辉光效果（Bloom）
- [ ] 色调映射（Tone Mapping）
- [ ] FXAA 抗锯齿
- [ ] 景深效果（DOF）

### 阴影系统 (REQ-027) 🔴

- [ ] 阴影贴图（Shadow Mapping）
- [ ] PCF 软阴影
- [ ] 多光源阴影
- [ ] 级联阴影贴图（CSM）
- [ ] 体积光（Volumetric Light）

### 性能优化 (REQ-024 续) 🟡

- [ ] 实例化渲染（Instanced Rendering）
- [ ] 纹理图集（Texture Atlas）
- [ ] GPU 剔除优化
- [ ] 遮挡剔除（Occlusion Culling）
- [ ] LOD（Level of Detail）

## 中期计划 (1-2 个月)

### 物理系统 (REQ-026) 🟡

- [ ] 简单的物理引擎集成
- [ ] 碰撞检测（AABB、球体）
- [ ] 重力和力场
- [ ] 刚体动力学
- [ ] 碰撞响应

### 材质系统增强 🟡

- [ ] PBR（Physically Based Rendering）材质
- [ ] 粗糙度贴图
- [ ] 金属度贴图
- [ ] 环境光遮蔽（AO）
- [ ] 材质编辑器

### 场景系统 🟡

- [ ] 场景图（Scene Graph）
- [ ] 层级变换
- [ ] 组件系统（Component System）
- [ ] 实体系统（Entity System）
- [ ] 场景加载器（支持多种格式）

## 长期计划 (3-6 个月)

### 高级渲染 🟢

- [ ] 延迟渲染（Deferred Rendering）
- [ ] 屏幕空间反射（SSR）
- [ ] 屏幕空间环境光遮蔽（SSAO）
- [ ] 全局光照（Global Illumination）
- [ ] 体素锥追踪（VXGI）

### 工具和编辑器 🟢

- [ ] 场景编辑器
- [ ] 材质编辑器
- [ ] 着色器编辑器
- [ ] 粒子编辑器
- [ ] 动画编辑器

### 内容创作 🟢

- [ ] 更多模型格式支持（GLTF, FBX）
- [ ] 动画系统
- [ ] 骨骼动画
- [ ] 蒙皮动画
- [ ] 粒子系统

## 当前待修复问题

### Bug

- [ ] 修复 Camera 类缺少 setFront/setUp 方法
- [ ] 修复包围盒旋转后的剔除计算不准确
- [ ] 修复法线贴图未正确应用

### 性能

- [ ] 优化网格数据传输（使用 glBufferSubData）
- [ ] 优化纹理加载（异步加载）
- [ ] 优化着色器编译（缓存）

### 代码质量

- [ ] 完善代码注释
- [ ] 添加更多单元测试
- [ ] 修复编译警告
- [ ] 代码风格统一

## 想法和建议

### 用户需求

- [ ] 支持导出截图
- [ ] 支持录制视频
- [ ] 支持保存配置
- [ ] 支持多语言

### 开发者需求

- [ ] 添加性能分析工具
- [ ] 添加着色器热重载
- [ ] 添加调试可视化
- [ ] 添加日志系统

### 实验性功能

- [ ] 光线追踪
- [ ] 体积渲染
- [ ] 程序化生成
- [ ] 神经渲染

## 技术债务

### 需要重构

- [ ] 统一错误处理机制
- [ ] 重构材质系统
- [ ] 重构资源管理
- [ ] 重构着色器系统

### 需要优化

- [ ] 减少内存分配
- [ ] 优化着色器性能
- [ ] 优化批处理
- [ ] 优化状态切换

### 需要文档

- [ ] 补充 API 文档
- [ ] 添加架构设计文档
- [ ] 添加性能优化指南
- [ ] 添加贡献指南

## 依赖升级计划

| 库 | 当前版本 | 目标版本 | 优先级 |
|------|----------|----------|--------|
| GLFW | 3.3.8 | 3.4+ | 🟢 |
| GLM | 0.9.9.8 | 1.0+ | 🟡 |
| Dear ImGui | docking | 最新稳定版 | 🟢 |
| nlohmann/json | 3.11.3 | 最新稳定版 | 🟢 |

## 参考资料

### 学习资源

- [Learn OpenGL](https://learnopengl.com/)
- [Real-Time Rendering](https://www.realtimerendering.com/)
- [GPU Gems](https://developer.nvidia.com/gpugems/)
- [Physically Based Rendering](https://pbr-book.org/)

### 开源项目

- [bgfx](https://github.com/bkaradzic/bgfx)
- [Ogre3D](https://github.com/OGRECave/ogre)
- [Filament](https://github.com/google/filament)
- [Magnum](https://github.com/mosra/magnum)

### 标准和规范

- [GLSL 规范](https://www.khronos.org/registry/OpenGL/index_gl.php)
- [OpenGL 规范](https://www.khronos.org/opengl/)
- [GLTF 规范](https://www.khronos.org/gltf/)
