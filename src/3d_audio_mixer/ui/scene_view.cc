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
} // namespace nui
