#include "render/opengl_context.hh"

namespace nrender
{
    bool OpenGL_Context::init(nwindow::IWindow* window)
    {
        RenderContext::init(window);

        if (!glfwInit()) 
        {
            return false;
        }

        auto glWindow = glfwCreateWindow(window->Width, window->Height, window->Title.c_str(), NULL, NULL);
        window->set_native_window(glWindow);

        if (!glWindow) 
        {
            return false;
        }

        glfwMakeContextCurrent(glWindow);

        return true;
    }

    void OpenGL_Context::pre_render()
    {
        // glViewport(0, 0, mWindow->Width, mWindow->Height);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
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
