#include "app/pch.h"
#include "ui/scene_view.hh"

#include <imgui/imgui.h>

namespace nui
{
    void SceneView::render()
    {

        // initalizer
        mFrameBuffer->bind();
        if (mIsLoaded)
        {
            for (auto& n : mSoundNodes)
            {
                n->render();
            }
        }
        mFrameBuffer->unbind();

        ImGui::Begin("Scene");

        if (mIsLoaded)
        {
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            mCamera->set_aspect(viewportPanelSize.x / viewportPanelSize.y);
            mCamera->update();
            

            // show rendered
            uint64_t textureID = mFrameBuffer->get_texture_id();
            ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(-1, -1));
        }

        ImGui::End();
    }

    bool SceneView::load_scene(nlohmann::json data)
    {
        // [TODO]: implement scene loading from JSON
        return false;
    }
} // namespace nui
