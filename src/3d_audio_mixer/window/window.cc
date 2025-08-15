#include "window/window.hh"

namespace nwindow
{
    bool Window::init(int width, int height, const std::string& title) 
    {
        Width = width;
        Height = height;
        Title = title;

        mRender->init(this);

        return isRunning;
    }

    void Window::render() 
    {
        // Pre-Render
        mRender->pre_render();

        // Render 
        
        
        // Post-Render
        mRender->post_render();


        // User input
        handle_input();
    }

    void Window::handle_input()
    {
        // [TODO]: list keyboard input for shortcut
        if (glfwGetKey(mWindow, GLFW_KEY_N) == GLFW_PRESS)
        {
            
        }
    }

    void Window::on_close() 
    {
        isRunning = false;
    }

    void Window::on_key(int key, int scancode, int action, int mods)
    {

    }

    void Window::on_resize(int width, int height)
    {
        Width = width;
        Height = height;

        render();
    }

    void Window::on_scroll(double delta)
    {

    }
} // namespace nwindow
