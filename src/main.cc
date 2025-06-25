#include <iostream>
#include <GLFW/glfw3.h>

int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(600, 480, "Window", NULL, NULL);

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

    }

    glfwTerminate();

    return 0;
}