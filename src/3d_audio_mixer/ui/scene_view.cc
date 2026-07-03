#include "app/pch.h"
#include "ui/scene_view.hh"

#include <imgui/imgui.h>

namespace nui
{
    void SceneView::render()
    {

        // initalizer
        mShader->use();
        mFrameBuffer->bind();
        if (mIsLoaded)
        {
            mShader->set_int(0, "uIsPoint");
            mShader->set_vec3(glm::vec3(0.55f, 0.55f, 0.55f), "uColor");
            mGrid->render();

            mShader->set_int(1, "uIsPoint");
            auto selected = mSelectedNode.lock();
            for (auto& n : mSoundNodes)
            {
                mShader->set_vec3(n == selected
                    ? glm::vec3(0.3f, 0.9f, 1.0f)
                    : glm::vec3(1.0f, 0.7f, 0.2f), "uColor");
                n->update(mShader.get());
                n->render();
            }
        }
        mFrameBuffer->unbind();

        ImGui::Begin("Scene");

        if (mIsLoaded)
        {
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            mCamera->set_aspect(viewportPanelSize.x / viewportPanelSize.y);
            mCamera->update(mShader.get());


            // show rendered
            uint64_t textureID = mFrameBuffer->get_texture_id();
            ImGui::Image(reinterpret_cast<void*>(textureID), viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));

            // node picking on left click inside the viewport image
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                ImVec2 rectMin = ImGui::GetItemRectMin();
                ImVec2 mouse = ImGui::GetMousePos();
                float u = (mouse.x - rectMin.x) / viewportPanelSize.x;
                float v = (mouse.y - rectMin.y) / viewportPanelSize.y;

                // image is displayed with uv0=(0,1), uv1=(1,0): panel top = NDC +1
                glm::vec2 ndc(u * 2.0f - 1.0f, 1.0f - v * 2.0f);
                pick_node(ndc, viewportPanelSize.x, viewportPanelSize.y);
            }
        }

        ImGui::End();
    }

    void SceneView::pick_node(const glm::vec2& ndc, float viewportWidth, float viewportHeight)
    {
        if (!mCamera) return;

        glm::mat4 viewProj = mCamera->get_projection_matrix() * mCamera->get_view_matrix();

        const float pickRadiusPx = 14.0f;
        std::shared_ptr<nelement::SoundNode> closest = nullptr;
        float closestDist = pickRadiusPx;

        for (auto& n : mSoundNodes)
        {
            glm::vec4 clip = viewProj * glm::vec4(n->get_position(), 1.0f);
            if (clip.w <= 0.0f) continue; // behind camera

            glm::vec2 nodeNdc = glm::vec2(clip.x, clip.y) / clip.w;
            glm::vec2 deltaPx((nodeNdc.x - ndc.x) * viewportWidth * 0.5f,
                              (nodeNdc.y - ndc.y) * viewportHeight * 0.5f);

            float dist = glm::length(deltaPx);
            if (dist < closestDist)
            {
                closestDist = dist;
                closest = n;
            }
        }

        mSelectedNode = closest;
        if (mNodeSelectedCallback)
        {
            mNodeSelectedCallback(closest);
        }
    }

    void SceneView::resize(int32_t width, int32_t height)
    {
        if (mFrameBuffer)
        {
            mFrameBuffer->create_buffers(width, height);
        }
    }

    bool SceneView::load_scene(nlohmann::json data)
    {
        // [TODO]: implement scene loading from JSON
        return false;
    }

    void SceneView::draw_grid()
    {

    }

    // Scene Controls
    void SceneView::on_mouse_move(double x, double y, nelement::EInputButton button)
    {
        if (mCamera)
        {
            mCamera->on_mouse_move(x, y, button);
        }
    }

    void SceneView::on_mouse_wheel(double delta)
    {
        if (mCamera)
        {
            mCamera->on_mouse_wheel(delta);
        }
    }
} // namespace nui
