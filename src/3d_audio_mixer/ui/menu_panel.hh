#pragma once

#include "elements/camera.hh"
#include "elements/sound_node.hh"

#include "app/pch.h"
#include <imgui/imgui.h>
#include <ImGuiFileBrowser/ImFileBrowser.h>
#include <nlohmann/json.hpp>

namespace nui
{
    class MenuPanel
    {
        private:
            ImGui::FileBrowser mFileDialog;
            std::function<void(const std::shared_ptr<nelement::Camera>& camera,
                const std::vector<std::shared_ptr<nelement::SoundNode>>& soundNodes)> mSceneLoaderCallback;

            // currently opened scene file (save target) and its original data
            std::string mCurrentFile;
            nlohmann::json mSceneData;
            std::function<void(const std::string& path, const nlohmann::json& baseData)> mSceneSaverCallback;

        public:
            MenuPanel()
            {
                mFileDialog.SetTitle("Select a file");
                mFileDialog.SetFileFilters({ ".json" });
            }

            // [TODO]: refactor to filepath for scalability
            void set_scene_loader_callback(const std::function<void(
                const std::shared_ptr<nelement::Camera>& camera,
                const std::vector<std::shared_ptr<nelement::SoundNode>>& soundNodes)>& callback)
            {
                mSceneLoaderCallback = callback;
            }

            void set_scene_saver_callback(const std::function<void(
                const std::string& path, const nlohmann::json& baseData)>& callback)
            {
                mSceneSaverCallback = callback;
            }

            void render();
    };
} // namespace nui
