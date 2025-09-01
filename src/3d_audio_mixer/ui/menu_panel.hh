#pragma once

#include "elements/camera.hh"
#include "elements/sound_node.hh"

#include "app/pch.h"
#include <imgui/imgui.h>
#include <ImGuiFileBrowser/ImFileBrowser.h>

namespace nui
{
    class MenuPanel
    {
        private:
            ImGui::FileBrowser mFileDialog;
            std::function<void(const std::shared_ptr<nelement::Camera>& camera,
                const std::vector<std::shared_ptr<nelement::SoundNode>>& soundNodes)> mSceneLoaderCallback;

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

            void render();
    };
} // namespace nui
