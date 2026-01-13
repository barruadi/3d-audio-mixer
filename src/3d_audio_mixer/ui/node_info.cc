#include "ui/node_info.hh"
#include "utils/services.hh"

namespace nui
{
    void NodeInfo::render()
    {
        ImGui::Begin("Node Info");

        if (mCurrentNode)
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
                mCurrentNode->play_sound();
            }
        }
        else
        {
            ImGui::Text("No node selected.");
        }

        ImGui::End();

        // File Browser
        mFileDialog.Display();
        if (mFileDialog.HasSelected() && mCurrentNode)
        {
            std::string selectedFile = mFileDialog.GetSelected().string();

            mCurrentNode->set_engine(naudio::get_audio_context());
            mCurrentNode->set_file(selectedFile);
            mCurrentNode->load_sound();

            mFileDialog.ClearSelected();
        }
    }
} // namespace nui