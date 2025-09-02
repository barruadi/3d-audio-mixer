#include "app/pch.h"
#include "ui/scene_view.hh"

#include <imgui/imgui.h>

namespace nui
{
    void SceneView::render()
    {

        // initalizer
        mShader->use();
        mFrameBuffer->bind();
        if (mIsLoaded)
        {
            mGrid->render();

            for (auto& n : mSoundNodes)
            {
                n->update(mShader.get());
                n->render();
            }
        }
        mFrameBuffer->unbind();

        ImGui::Begin("Scene");

        if (mIsLoaded)
        {
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            mCamera->set_aspect(viewportPanelSize.x / viewportPanelSize.y);
            mCamera->update(mShader.get());
            

            // show rendered
            uint64_t textureID = mFrameBuffer->get_texture_id();
            ImGui::Image(reinterpret_cast<void*>(textureID), viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::End();
    }

    void SceneView::resize(int32_t width, int32_t height)
    {
        if (mFrameBuffer)
        {
            mFrameBuffer->create_buffers(width, height);
        }
    }

    bool SceneView::load_scene(nlohmann::json data)
    {
        // [TODO]: implement scene loading from JSON
        return false;
    }

    void SceneView::draw_grid()
    {

    }

    // Scene Controls
    void SceneView::on_mouse_move(double x, double y, nelement::EInputButton button)
    {
        if (mCamera)
        {
            mCamera->on_mouse_move(x, y, button);
        }
    }

    void SceneView::on_mouse_wheel(double delta)
    {
        if (mCamera)
        {
            mCamera->on_mouse_wheel(delta);
        }
    }
} // namespace nui
