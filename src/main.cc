#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(600, 480, "Window", NULL, NULL);

    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

    }

    glfwTerminate();

    return 0;
}