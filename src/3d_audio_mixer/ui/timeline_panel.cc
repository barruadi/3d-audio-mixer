#include "app/pch.h"
#include "ui/timeline_panel.hh"

#include <cstdio>

namespace nui
{
    void TimelinePanel::render()
    {
        ImGui::SetNextWindowSize(ImVec2(900, 220), ImGuiCond_FirstUseEver);
        ImGui::Begin("Timeline");

        if (!mNodesProvider || mNodesProvider().empty())
        {
            ImGui::Text("No audio nodes in scene.");
            ImGui::End();
            return;
        }

        const auto& nodes = mNodesProvider();
        for (size_t i = 0; i < nodes.size(); i++)
        {
            ImGui::PushID(static_cast<int>(i));
            render_track(nodes[i]);
            ImGui::PopID();
        }

        ImGui::End();
    }

    void TimelinePanel::render_track(const std::shared_ptr<nelement::SoundNode>& node)
    {
        ImGui::Text("%s", node->get_name().c_str());

        if (node->get_file_path().empty())
        {
            ImGui::SameLine();
            ImGui::TextDisabled("<no file>");
            ImGui::Separator();
            return;
        }

        // Load lazily so length/cursor are available before first play,
        // but never retry a failed file (avoids per-frame error logging)
        if (!node->is_loaded() && !node->has_load_failed())
        {
            node->load_sound();
        }

        if (node->has_load_failed())
        {
            ImGui::SameLine();
            ImGui::TextDisabled("<failed to load: %s>", node->get_file_path().c_str());
            ImGui::Separator();
            return;
        }

        float cursor = node->get_cursor_seconds();
        float length = node->get_length_seconds();

        if (ImGui::Button(node->is_playing() ? "Pause" : "Play"))
        {
            if (node->is_playing())
            {
                node->pause_sound();
            }
            else if (cursor > 0.0f && cursor < length)
            {
                node->resume_sound();
            }
            else
            {
                node->play_sound();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            node->stop_sound();
        }

        ImGui::SameLine();
        char timeText[32];
        std::snprintf(timeText, sizeof(timeText), "%02d:%02d / %02d:%02d",
            static_cast<int>(cursor) / 60, static_cast<int>(cursor) % 60,
            static_cast<int>(length) / 60, static_cast<int>(length) % 60);

        ImGui::SetNextItemWidth(-120.0f);
        if (ImGui::SliderFloat("##seek", &cursor, 0.0f, length, timeText))
        {
            node->seek_to_second(cursor);
        }

        ImGui::Separator();
    }
} // namespace nui
