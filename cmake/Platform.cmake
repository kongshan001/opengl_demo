# Platform.cmake
# 平台特定配置

# 根据平台设置链接库
if(APPLE)
    set(PLATFORM_LIBRARIES
        ${OPENGL_LIBRARIES}
        glfw
        glm::glm
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
elseif(UNIX)
    set(PLATFORM_LIBRARIES
        ${OPENGL_LIBRARIES}
        glfw
        glm::glm
        -ldl
        -lpthread
    )
elseif(WIN32)
    set(PLATFORM_LIBRARIES
        ${OPENGL_LIBRARIES}
        glfw
        glm::glm
        opengl32
    )
endif()
