# 变更日志

本文档记录项目的所有重要变更。

## [Unreleased]

### 新增

### 改进

### 修复

### 移除

## [2026-02-22]

### 新增

**REQ-024: 视锥体剔除 (Performance)**
- 添加 Frustum 类处理视锥体计算
- 支持 AABB 包围盒剔除
- 实时统计剔除率和性能数据
- ImGui 中显示剔除统计
- 可开关视锥体剔除功能

**REQ-023: 场景序列化**
- 创建 SceneSerializer 类处理 JSON 序列化
- 支持保存/加载场景配置（光源、显示、摄像机）
- 添加 nlohmann/json 依赖
- Ctrl+S 保存场景，Ctrl+O 加载场景
- 文件菜单支持保存/加载/退出

**REQ-022: 多纹理支持**
- multi_light.fs 支持漫反射、镜面反射、法线贴图
- Application 加载镜面反射纹理
- 添加纹理控制 ImGui 窗口
- 支持 3 个纹理单元（TEXTURE0-2）
- 镜面反射强度可纹理控制

**REQ-021: 新几何体**
- MeshUtils::createTorus() - 圆环生成
- MeshUtils::createCapsule() - 胶囊体生成
- 支持自定义半径、高度、细分段数
- 正确的法线和纹理坐标计算

**REQ-020: ImGui 调试界面**
- 集成 Dear ImGui (docking 分支)
- 添加性能统计窗口（FPS、帧时间）
- 添加光源控制窗口（实时调整参数）
- 添加菜单栏（窗口和显示菜单）
- Tab 键切换 UI 显示
- 使用 FetchContent 自动下载 ImGui

**REQ-019: 键盘快捷键改进**
- 数字键使用按键释放检测，避免连续触发
- 添加 F1 帮助显示（控制台输出）
- 更新 README 添加 F1 说明

**REQ-018: 多光源支持**
- 创建 multi_light.fs 多光源着色器
- 支持 4 个点光源同时照射
- 实现距离衰减 (constant/linear/quadratic)
- 3 个不同颜色的光源 (白/蓝/橙)
- 各光源独立轨道旋转动画
- 更新 README 光源说明

**REQ-017: 动态光源旋转动画**
- 光源围绕场景圆形轨道旋转
- 添加 L 键切换光源动画开关
- 可配置轨道半径、高度、速度
- 更新 README 控制说明

**REQ-016: 完整 Phong 光照模型**
- 创建 phong.fs 着色器（环境光 + 漫反射 + 镜面反射）
- 更新 Application 使用 Phong 光照
- 添加 Material 和 Light 结构体 uniform
- 为每个几何体设置不同的材质属性
- 22 个相关测试全部通过

## [2026-02-21]

### 新增

**REQ-011 ~ REQ-015**
- REQ-011: Camera 和 CMesh 扩展单元测试
- REQ-012: 添加圆柱体和圆锥体生成
- REQ-013: 展示多种几何体（立方体、球体、圆柱体、圆锥体）
- REQ-014: 添加数字键切换显示模式和线框模式
- REQ-015: 添加控制说明到 README

### 改进

- 使用 FetchContent 自动下载 GLFW 和 GLM
- 添加架构文档和核心类职责文档
- 修复顶点属性步长错误

### 提交记录

- `test: 添加 Camera 和 CMesh 扩展单元测试 (REQ-011)`
- `feat(mesh): 添加圆柱体和圆锥体生成 (REQ-012)`
- `feat(scene): 展示多种几何体（立方体、球体、圆柱体、圆锥体）(REQ-013)`
- `feat(input): 添加数字键切换显示模式和线框模式 (REQ-014)`
- `docs: 添加控制说明到 README (REQ-015)`
- `docs: 添加架构文档和核心类职责文档`

## [2026-02-20]

### 修复

**代码审查问题修复**
- Shader 错误处理（抛异常而非静默失败）
- 修复 CMake CMP0072 警告
- ModelLoader 使用智能指针
- 缓存 uniform location
- 完善 API 文档（Doxygen 注释）

### 新增

**REQ-001 ~ REQ-010**
- REQ-001: 改进纹理立方体渲染效果
- REQ-002 ~ REQ-010: 多角色协作系统迭代

### 技术改进

- 多角色协作系统设计
- 创建 4 个角色 Skill（product-owner, developer, code-reviewer, qa-engineer）
- 创建 multi-agent-workflow 协作流程文档
- 完整验证 Multi-Agent 协作流程

## 版本说明

### 版本号规则

遵循 [语义化版本](https://semver.org/lang/zh-CN/)：
- 主版本号：不兼容的 API 修改
- 次版本号：向下兼容的功能性新增
- 修订号：向下兼容的问题修正

### 发布计划

当前处于 **开发阶段**，尚未发布正式版本。

### 未来计划

- v1.0.0: 核心功能稳定版
- v1.1.0: 后期处理（Bloom、FXAA）
- v2.0.0: 物理引擎集成
