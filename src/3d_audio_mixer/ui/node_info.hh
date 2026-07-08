#pragma once

#include "elements/sound_node.hh"
#include "elements/listener.hh"

#include "app/pch.h"
#include <imgui/imgui.h>
#include <ImGuiFileBrowser/ImFileBrowser.h>

namespace nui
{
    class NodeInfo
    {
        private:
            ImGui::FileBrowser mFileDialog;
            std::weak_ptr<nelement::SoundNode> mCurrentNode;
            std::weak_ptr<nelement::Listener> mCurrentListener;

            void render_node(const std::shared_ptr<nelement::SoundNode>& node);
            void render_listener(const std::shared_ptr<nelement::Listener>& listener);

        public:
            NodeInfo()
            {
                mFileDialog.SetTitle("Select a file");
                mFileDialog.SetFileFilters({ ".wav" });
            }

            void set_current_node(const std::shared_ptr<nelement::SoundNode>& node)
            {
                mCurrentNode = node;
                if (node)
                {
                    mCurrentListener.reset();
                }
            }

            void set_current_listener(const std::shared_ptr<nelement::Listener>& listener)
            {
                mCurrentListener = listener;
                if (listener)
                {
                    mCurrentNode.reset();
                }
            }

            void render();
    };
} // namespace nui
