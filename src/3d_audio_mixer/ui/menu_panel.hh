#pragma once

#include "app/pch.h"
#include <imgui/imgui.h>
#include <ImGuiFileBrowser/ImFileBrowser.h>

namespace nui
{
    class MenuPanel
    {
        private:
            ImGui::FileBrowser mFileDialog;
            std::function<void(const std::string&)> mSceneLoaderCallback;

        public:
            MenuPanel()
            {
                mFileDialog.SetTitle("Select a file");
                mFileDialog.SetFileFilters({ ".json" });
            }

            void set_scene_loader_callback(const std::function<void(const std::string&)>& callback)
            {
                mSceneLoaderCallback = callback;
            }

            void render();
    };
} // namespace nui
