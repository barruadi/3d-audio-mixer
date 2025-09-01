#include "window/window.hh"

namespace nwindow
{
    bool Window::init(int width, int height, const std::string& title) 
    {
        Width = width;
        Height = height;
        Title = title;

        mRender->init(this);
        mUI->init(this);

        mMenuPanel = std::make_unique<nui::MenuPanel>();
        mSceneView = std::make_unique<nui::SceneView>();

        // Set the scene loader callback
        // [TODO]: cleanup and fix pointer issue
        mMenuPanel->set_scene_loader_callback([this](
            const std::shared_ptr<nelement::Camera>& camera,
            const std::vector<std::shared_ptr<nelement::SoundNode>>& soundNodes) 
        {
            std::vector<std::unique_ptr<nelement::SoundNode>> uniqueSoundNodes;
            for (const auto& soundNode : soundNodes) {
                uniqueSoundNodes.push_back(std::make_unique<nelement::SoundNode>(*soundNode));
            }
            mSceneView->set_scene(
                std::make_unique<nelement::Camera>(*camera),
                std::move(uniqueSoundNodes)
            );
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
