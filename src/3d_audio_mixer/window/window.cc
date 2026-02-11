#include "window/window.hh"
#include "utils/services.hh"

namespace nwindow
{
    bool Window::init(int width, int height, const std::string& title) 
    {
        Width = width;
        Height = height;
        Title = title;

        mRender->init(this);
        mUI->init(this);
        mAudioContext->init();
        
        naudio::Services::instance().sAudioContext = mAudioContext;

        mMenuPanel = std::make_unique<nui::MenuPanel>();
        mSceneView = std::make_unique<nui::SceneView>();
        mNodeInfo = std::make_unique<nui::NodeInfo>();

        // Set the scene loader callback - uses shared ownership
        mMenuPanel->set_scene_loader_callback([this](
            const std::shared_ptr<nelement::Camera>& camera,
            const std::vector<std::shared_ptr<nelement::SoundNode>>& soundNodes)
        {
            mSceneView->set_scene(camera, soundNodes);
        });

        return isRunning;
    }

    void Window::render() 
    {
        // Pre-Render
        mRender->pre_render();
        mUI->pre_render();

        // Render 
        mMenuPanel->render();
        mSceneView->render();
        mNodeInfo->render();

        // Post-Render
        mUI->post_render();
        mRender->post_render();


        // User input
        handle_input();
    }

    void Window::handle_input()
    {
        // [TODO]: list keyboard input for shortcut
        if (glfwGetKey(mWindow, GLFW_KEY_N) == GLFW_PRESS)
        {
            // Select first node for testing
            auto node = mSceneView->get_node(0);
            mNodeInfo->set_current_node(node);
        }

        double x, y;
        glfwGetCursorPos(mWindow, &x, &y);
        mSceneView->on_mouse_move(x, y, nelement::Input::get_mouse_button(mWindow));
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

        if (mSceneView) mSceneView->resize(width, height);
        render();
    }

    void Window::on_scroll(double delta)
    {
        mSceneView->on_mouse_wheel(delta);
    }
} // namespace nwindow
