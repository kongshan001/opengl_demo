# OpenGL Demo

一个最小可运行的OpenGL demo，绘制一个橙色三角形。

## 依赖安装

```bash
pip install -r requirements.txt
```

## 运行

```bash
python3 opengl_demo.py
```

## 项目结构

```
opengl_demo/
├── opengl_demo.py    # 主程序
├── requirements.txt  # Python依赖
├── main.cpp          # C++版本（可选）
├── CMakeLists.txt    # CMake构建配置（C++版本用）
└── README.md         # 说明文档
```

## 功能

- 创建OpenGL窗口（800x600）
- 绘制一个橙色三角形
- 渲染循环处理窗口事件
- 使用现代OpenGL 3.3核心配置
