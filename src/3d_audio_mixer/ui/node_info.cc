#include "ui/node_info.hh"
#include "utils/services.hh"

namespace nui
{
    void NodeInfo::render()
    {
        ImGui::Begin("Node Info");

        // Lock the weak_ptr to get a shared_ptr for safe access
        auto node = mCurrentNode.lock();

        if (node)
        {
            // Display node information
            ImGui::Text("Sound Node Information");
            // [TODO]: Add more detailed information and editable fields

            if (ImGui::Button("Load Sound File"))
            {
                mFileDialog.Open();
            }

            if (ImGui::Button("Play Sound"))
            {
                node->play_sound();
            }
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
} // namespace nui