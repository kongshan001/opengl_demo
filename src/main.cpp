/**
 * @file main.cpp
 * @brief OpenGL Demo 入口点
 * 
 * 极简 main 函数，所有逻辑委托给 Application 类。
 */

#include "core/Application.h"

int main() {
    // 创建配置（可选：自定义参数）
    AppConfig config;
    config.title = "OpenGL Demo - Modular";
    
    // 创建应用
    Application app(config);
    
    // 初始化
    if (!app.initialize()) {
        return -1;
    }
    
    // 运行主循环
    app.run();
    
    // 析构时自动清理
    return 0;
}
