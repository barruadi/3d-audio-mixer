#include "app/pch.h"
#include "ui/menu_panel.hh"
#include "utils/scene_loader.hh"
#include "utils/file_io.hh"

#include <nlohmann/json.hpp>

namespace nui
{
    void MenuPanel::render()
    {
        ImGui::Begin("Menu Panel");

        if (ImGui::Button("Open File"))
        {
            mFileDialog.Open();
        }

        ImGui::End();

        // File Browser
        mFileDialog.Display();
        if (mFileDialog.HasSelected())
        {
            nlohmann::json data;
            std::string selectedFile = mFileDialog.GetSelected().string();
            if (nutils::FileIO::read_json(selectedFile, data))
            {
                nutils::SceneLoader loader;
                loader.set_data(data);

                std::shared_ptr<nelement::Camera> camera;
                if (!loader.load_camera(&camera))
                {
                    std::cout << "[LOAD ERROR] Failed to load camera data." << std::endl;
                    return;
                }

                std::vector<std::shared_ptr<nelement::SoundNode>> soundNodes;
                if (!loader.load_sound_nodes(&soundNodes))
                {
                    std::cout << "[LOAD ERROR] Failed to load sound nodes." << std::endl;
                    return;
                }

                if (mSceneLoaderCallback)
                {
                    mSceneLoaderCallback(camera, soundNodes);
                }
            }
            mFileDialog.ClearSelected();
        }
    }
} // namespace nui
