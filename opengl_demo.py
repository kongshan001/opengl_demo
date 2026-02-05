import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GL.shaders import compileProgram, compileShader


def main():
    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)
    pygame.display.set_caption("OpenGL Demo")

    glViewport(0, 0, 800, 600)
    glClearColor(0.2, 0.3, 0.3, 1.0)

    vertex_src = """
    #version 120
    attribute vec3 position;
    void main() {
        gl_Position = vec4(position, 1.0);
    }
    """

    fragment_src = """
    #version 120
    void main() {
        gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    }
    """

    shader = compileProgram(
        compileShader(vertex_src, GL_VERTEX_SHADER),
        compileShader(fragment_src, GL_FRAGMENT_SHADER),
    )

    position_location = glGetAttribLocation(shader, "position")

    vertices = [0.0, 0.5, 0.0, -0.5, -0.5, 0.0, 0.5, -0.5, 0.0]

    VBO = glGenBuffers(1)

    glBindBuffer(GL_ARRAY_BUFFER, VBO)
    glBufferData(
        GL_ARRAY_BUFFER,
        len(vertices) * 4,
        (ctypes.c_float * len(vertices))(*vertices),
        GL_STATIC_DRAW,
    )

    glVertexAttribPointer(
        position_location, 3, GL_FLOAT, GL_FALSE, 12, ctypes.c_void_p(0)
    )
    glEnableVertexAttribArray(position_location)

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glUseProgram(shader)
        glBindBuffer(GL_ARRAY_BUFFER, VBO)
        glVertexAttribPointer(
            position_location, 3, GL_FLOAT, GL_FALSE, 12, ctypes.c_void_p(0)
        )
        glEnableVertexAttribArray(position_location)
        glDrawArrays(GL_TRIANGLES, 0, 3)
        pygame.display.flip()
        pygame.time.wait(10)

    pygame.quit()


if __name__ == "__main__":
    import ctypes

    main()
