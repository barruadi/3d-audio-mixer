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

        public:
            MenuPanel()
            {
                mFileDialog.SetTitle("Select a file");
                mFileDialog.SetFileFilters({ ".json" });
            }

            void render();
    };
} // namespace nui
