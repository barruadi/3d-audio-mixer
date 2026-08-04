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

        ImGui::SameLine();
        if (ImGui::Button("Save"))
        {
            if (mCurrentFile.empty())
            {
                std::cout << "[INFO] No scene file opened, nothing to save." << std::endl;
            }
            else if (mSceneSaverCallback)
            {
                mSceneSaverCallback(mCurrentFile, mSceneData);
            }
        }

        ImGui::SameLine();
        if (mIsRendering)
        {
            ImGui::BeginDisabled();
            ImGui::Button("Rendering...");
            ImGui::EndDisabled();
        }
        else if (ImGui::Button("Render Scene"))
        {
            if (mRenderCallback)
            {
                mIsRendering = true;
                std::string outputPath = "render.wav";
                if (!mCurrentFile.empty())
                {
                    // Put render output next to the scene file
                    std::string dir = mCurrentFile.substr(0, mCurrentFile.find_last_of("/\\") + 1);
                    outputPath = dir + "render.wav";
                }
                mRenderCallback(outputPath);
                mIsRendering = false;
                std::cout << "[INFO] Render complete: " << outputPath << std::endl;
            }
            else
            {
                std::cerr << "[ERROR] No render callback set" << std::endl;
            }
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

                // falls back to a default listener at origin when missing
                std::shared_ptr<nelement::Listener> listener;
                if (!loader.load_listener(&listener))
                {
                    std::cout << "[INFO] No listener in scene file, using default." << std::endl;
                }

                if (mSceneLoaderCallback)
                {
                    mSceneLoaderCallback(camera, soundNodes, listener);
                }

                // remember as the save target
                mCurrentFile = selectedFile;
                mSceneData = data;
            }
            mFileDialog.ClearSelected();
        }
    }
} // namespace nui
