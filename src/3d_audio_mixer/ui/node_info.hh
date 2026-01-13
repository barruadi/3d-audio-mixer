#pragma once

#include "elements/sound_node.hh"

#include "app/pch.h"
#include <imgui/imgui.h>
#include <ImGuiFileBrowser/ImFileBrowser.h>

namespace nui
{
    class NodeInfo
    {
        private:
            ImGui::FileBrowser mFileDialog;
            nelement::SoundNode* mCurrentNode;

        public:
            NodeInfo():
                mCurrentNode(nullptr)
            {
                mFileDialog.SetTitle("Select a file");
                mFileDialog.SetFileFilters({ ".wav" });
            }

            void set_current_node(nelement::SoundNode* node)
            {
                mCurrentNode = node;
            }

            void render();
    };
} // namespace nui
