#include "app/pch.h"
#include "ui/scene_view.hh"

#include <imgui/imgui.h>

namespace nui
{
    void SceneView::render()
    {

        // if (mIsLoaded)
        // {
        //     for (auto& n : mSoundNodes)
        //     {
        //         n->render();
        //     }
        // }

        ImGui::Begin("Scene");

        if (mIsLoaded)
        {
            ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", 
                        mCamera->get_position().x, 
                        mCamera->get_position().y, 
                        mCamera->get_position().z);
            ImGui::Text("Number of Sound Nodes: %zu", mSoundNodes.size());
        }
        else
        {
            ImGui::Text("No scene loaded.");
        }

        ImGui::End();
    }

    bool SceneView::load_scene(nlohmann::json data)
    {
        // [TODO]: implement scene loading from JSON
        return false;
    }
} // namespace nui
