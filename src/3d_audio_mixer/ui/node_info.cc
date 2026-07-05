#include "ui/node_info.hh"
#include "utils/services.hh"

#include <cstdio>

namespace nui
{
    void NodeInfo::render()
    {
        ImGui::Begin("Node Info");

        // Lock the weak_ptrs to get shared_ptrs for safe access
        auto node = mCurrentNode.lock();
        auto listener = mCurrentListener.lock();

        if (node)
        {
            render_node(node);
        }
        else if (listener)
        {
            render_listener(listener);
        }
        else
        {
            ImGui::Text("No node selected.");
        }

        ImGui::End();

        // File Browser
        mFileDialog.Display();
        if (mFileDialog.HasSelected() && node)
        {
            std::string selectedFile = mFileDialog.GetSelected().string();

            node->set_engine(naudio::get_audio_context());
            node->set_file(selectedFile);
            node->load_sound();

            mFileDialog.ClearSelected();
        }
    }

    void NodeInfo::render_node(const std::shared_ptr<nelement::SoundNode>& node)
    {
        ImGui::Text("Sound Node Information");
        ImGui::Separator();

        // name
        char nameBuf[128];
        std::snprintf(nameBuf, sizeof(nameBuf), "%s", node->get_name().c_str());
        if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
        {
            node->set_name(nameBuf);
        }

        // sound file
        const std::string& file = node->get_file_path();
        ImGui::Text("File: %s", file.empty() ? "<none>" : file.c_str());
        if (ImGui::Button("Load Sound File"))
        {
            mFileDialog.Open();
        }

        // properties
        glm::vec3 position = node->get_position();
        bool positionChanged = false;
        positionChanged |= ImGui::SliderFloat("X", &position.x, -10.0f, 10.0f);
        positionChanged |= ImGui::SliderFloat("Y", &position.y, -10.0f, 10.0f);
        positionChanged |= ImGui::SliderFloat("Z", &position.z, -10.0f, 10.0f);
        if (positionChanged)
        {
            node->set_position(position);
        }

        float volume = node->get_volume();
        if (ImGui::SliderFloat("Volume", &volume, 0.0f, 2.0f))
        {
            node->set_volume(volume);
        }

        float pan = node->get_pan();
        if (ImGui::SliderFloat("Pan", &pan, -1.0f, 1.0f))
        {
            node->set_pan(pan);
        }

        bool looping = node->is_looping();
        if (ImGui::Checkbox("Looping", &looping))
        {
            node->set_looping(looping);
        }

        ImGui::Separator();

        if (ImGui::Button("Play Sound"))
        {
            node->play_sound();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            node->stop_sound();
        }
    }

    void NodeInfo::render_listener(const std::shared_ptr<nelement::Listener>& listener)
    {
        ImGui::Text("Listener Information");
        ImGui::Separator();

        ImGui::Text("Name: %s", listener->get_name().c_str());

        glm::vec3 position = listener->get_position();
        bool positionChanged = false;
        positionChanged |= ImGui::SliderFloat("X", &position.x, -10.0f, 10.0f);
        positionChanged |= ImGui::SliderFloat("Y", &position.y, -10.0f, 10.0f);
        positionChanged |= ImGui::SliderFloat("Z", &position.z, -10.0f, 10.0f);
        if (positionChanged)
        {
            listener->set_position(position);
        }
    }
} // namespace nui
