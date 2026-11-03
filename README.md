## nanodraw.h

nanodraw.h is a simple 2D graphics library. It is written in C99 as an STB-style single header library and uses OpenGL. If you want to draw 2D graphics to the screen, nanodraw.h is one way you could do it.

It's lighter than SDL and Raylib. It removes any need for GLAD. It's window-system agnostic. It saves you from having to deal with OpenGL bs or--even worse--write 600 lines of code to draw triangles on the screen in a somewhat optimized manner.

```c
#include <stdio.h>
#include <GLFW/glfw3.h>

#define NANODRAW_IMPLEMENTATION
#include "nanodraw.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    (void)window;
    nd_update_viewport(width, height);
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "hello, nanodraw.h", NULL, NULL);
    if (!window) {
        puts("failed to open window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!nd_init(800, 600, (void *)glfwGetProcAddress)) {
        puts("failed to init nanodraw");
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        nd_clear_framebuffer(ND_FRAMEBUFFER_NULL, ND_BLACK);

        nd_draw_rectangle(
            ND_FRAMEBUFFER_NULL,
            ND_BLEND_NONE,
            200, 200,
            400, 200,
            ND_WHITE
        );

        if (!nd_submit_draw_queue()) {
            puts("failed to draw geometry to the screen");
            nd_deinit();
            glfwTerminate();
            return -1;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    nd_deinit();
    glfwTerminate();
    return 0;
```

## Why use this over Raylib/SDL/etc?

nanodraw.h has several advantages over traditional 2D graphics libraries:
- It's small (< 1000 LOC, roughly the same size as the [raylib cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html))
- It only depends on OpenGL and libc
  - You pass in a function to load OpenGl pointers, and everything works
- It has a simple API that you can learn in 10 minutes (only 19 functions in the header section)
- It abstracts away annoying OpenGL details like global state and flipped coordinates that remain in other libraries
  - Raylib will draw things upside down when you draw to a texture
  - In SDL, if you want to draw a blue rectangle, you have to bind the color blue and *then* draw a rectangle
