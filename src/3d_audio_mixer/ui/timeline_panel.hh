#pragma once

#include "elements/sound_node.hh"

#include "app/pch.h"
#include <imgui/imgui.h>
#include <functional>
#include <vector>

namespace nui
{
    class TimelinePanel
    {
        private:
            // provider keeps rows in sync with the scene, including nodes
            // added at runtime via the scene context menu
            std::function<const std::vector<std::shared_ptr<nelement::SoundNode>>&()> mNodesProvider;

            void render_track(const std::shared_ptr<nelement::SoundNode>& node);

        public:
            void set_nodes_provider(const std::function<
                const std::vector<std::shared_ptr<nelement::SoundNode>>&()>& provider)
            {
                mNodesProvider = provider;
            }

            void render();
    };
} // namespace nui
