#include "app/pch.h"
#include "render/opengl_context.hh"

namespace nrender
{

    static void on_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto pWindow = static_cast<nwindow::IWindow*>(glfwGetWindowUserPointer(window));
        pWindow->on_key(key, scancode, action, mods);
    }

    static void on_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto pWindow = static_cast<nwindow::IWindow*>(glfwGetWindowUserPointer(window));
        pWindow->on_scroll(yoffset);
    }

    static void on_window_size_callback(GLFWwindow* window, int width, int height)
    {
        auto pWindow = static_cast<nwindow::IWindow*>(glfwGetWindowUserPointer(window));
        pWindow->on_resize(width, height);
    }

    static void on_window_close_callback(GLFWwindow* window)
    {
        nwindow::IWindow* pWindow = static_cast<nwindow::IWindow*>(glfwGetWindowUserPointer(window));
        pWindow->on_close();
    }

    bool OpenGL_Context::init(nwindow::IWindow* window)
    {
        RenderContext::init(window);

        if (!glfwInit()) 
        {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        auto glWindow = glfwCreateWindow(window->Width, window->Height, window->Title.c_str(), NULL, NULL);
        window->set_native_window(glWindow);

        if (!glWindow) 
        {
            return false;
        }

        glfwSetWindowUserPointer(glWindow, window);

        glfwSetKeyCallback(glWindow, on_key_callback);
        glfwSetScrollCallback(glWindow, on_scroll_callback);
        glfwSetWindowSizeCallback(glWindow, on_window_size_callback);
        glfwSetWindowCloseCallback(glWindow, on_window_close_callback);

        glfwMakeContextCurrent(glWindow);

        if (!gladLoadGL()) {
            std::cerr << "Failed to initialize GLAD!" << std::endl;
            return false;
        }

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "[GLAD] Failed to initialize OpenGL context" << std::endl;
            return false;
        }

        glEnable(GL_DEPTH_TEST);

        return true;
    }

    void OpenGL_Context::pre_render()
    {
        glViewport(0, 0, mWindow->Width, mWindow->Height);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void OpenGL_Context::post_render()
    {
        glfwPollEvents();
        glfwSwapBuffers((GLFWwindow*) mWindow->get_native_window());
    }

    void OpenGL_Context::end()
    {
        glfwDestroyWindow((GLFWwindow*) mWindow->get_native_window());
        glfwTerminate();
    }
} // namespace nrender
