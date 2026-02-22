# 多角色协作开发流程

## 角色定义

### 🎯 策划 (Product Owner)
- **职责**：提出需求、定义优先级、验收成果
- **产出**：需求文档 (docs/requirements/)
- **权限**：可修改需求文档和优先级

### 👨‍💻 程序 (Developer)
- **职责**：使用 Claude Code / OpenClaw skills 实现需求
- **产出**：代码、接口文档、架构文档
- **工具**：coding-agent skill
- **要求**：高质量、高效率、遵循代码规范

### 👮 代码审核人 (Code Reviewer)
- **职责**：审核代码质量、安全性、性能
- **产出**：审核报告 (docs/reviews/)
- **检查项**：
  - 代码规范
  - 内存安全
  - 性能问题
  - 架构合理性

### 🧪 QA (Quality Assurance)
- **职责**：编写单元测试、集成测试
- **目标**：100% 测试覆盖率
- **产出**：测试代码、测试报告
- **要求**：每个功能必须有对应测试

---

## 工作流程

```
┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
│  策划   │───>│  程序   │───>│  审核   │───>│   QA    │
│ 提需求  │    │  实现   │    │  代码   │    │  测试   │
└─────────┘    └─────────┘    └─────────┘    └─────────┘
     │              │              │              │
     v              v              v              v
  需求文档       代码+文档       审核报告       测试报告
```

### 每个需求的完整周期：

1. **策划** → 创建需求文档 `docs/requirements/REQ-XXX.md`
2. **程序** → 实现功能 + 更新文档
3. **审核** → 审核代码，输出审核报告
4. **QA** → 编写测试，确保覆盖率
5. **提交** → 整理文档，推送远程仓库

---

## 文档要求

每次提交必须包含：

### 1. 接口文档
- 位置：`docs/api/`
- 内容：新增/修改的 API 说明

### 2. 架构文档
- 位置：`doc/architecture.md`
- 内容：架构变更说明

### 3. 核心类职责
- 位置：`doc/core-classes.md`
- 内容：新增/修改类的职责说明

---

## Git 提交规范

```
[角色] 类型: 简短描述

详细描述...

相关需求: REQ-XXX
审核人: @reviewer
测试覆盖: XX%
```

### 类型：
- `feat`: 新功能
- `fix`: 修复
- `docs`: 文档
- `refactor`: 重构
- `test`: 测试
- `chore`: 构建/工具

---

## 会话管理

使用 OpenClaw sessions_spawn 创建独立角色会话：

```bash
# 策划会话
sessions_spawn --agent-id planner --label "策划-REQ-001"

# 程序会话
sessions_spawn --agent-id developer --label "程序-REQ-001"

# 审核会话
sessions_spawn --agent-id reviewer --label "审核-REQ-001"

# QA会话
sessions_spawn --agent-id qa --label "QA-REQ-001"
```
