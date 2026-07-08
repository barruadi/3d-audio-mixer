#pragma once

#include "elements/sound_node.hh"
#include "elements/listener.hh"
#include "elements/camera.hh"
#include "elements/grid.hh"
#include "render/opengl_buffer_manager.hh"
#include "shader/shader.hh"
#include "utils/services.hh"
#include <functional>
#include <vector>
#include <nlohmann/json.hpp>

namespace nui
{
    class SceneView
    {
        private:
            bool mIsLoaded;

            // Scene objects
            std::unique_ptr<nelement::Grid> mGrid;
            std::shared_ptr<nelement::Camera> mCamera;
            std::vector<std::shared_ptr<nelement::SoundNode>> mSoundNodes;
            std::shared_ptr<nelement::Listener> mListener;
            
            // framebuffer
            std::unique_ptr<nrender::OpenGL_FrameBuffer> mFrameBuffer;

            // shader
            std::unique_ptr<nshader::Shader> mShader;

            // selection
            std::weak_ptr<nelement::SoundNode> mSelectedNode;
            std::weak_ptr<nelement::Listener> mSelectedListener;
            std::function<void(const std::shared_ptr<nelement::SoundNode>&)> mNodeSelectedCallback;
            std::function<void(const std::shared_ptr<nelement::Listener>&)> mListenerSelectedCallback;

            void pick_node(const glm::vec2& ndc, float viewportWidth, float viewportHeight);

        public:
            SceneView():
                mIsLoaded(false),
                mCamera(nullptr),
                mSoundNodes()
            {
                mGrid = std::make_unique<nelement::Grid>();
                mFrameBuffer = std::make_unique<nrender::OpenGL_FrameBuffer>();
                mFrameBuffer->create_buffers(800, 600);

                mShader = std::make_unique<nshader::Shader>();
                mShader->load("src/shaders/node_vert.shader", "src/shaders/node_frag.shader");
                mCamera = std::make_shared<nelement::Camera>();
            }

            ~SceneView()
            {
                // Clean up resources
            }

            void render();

            void resize(int32_t width, int32_t height);

            // [TODO]: make universal in case of other format
            bool load_scene(nlohmann::json data);

            void set_scene(std::shared_ptr<nelement::Camera> camera,
                           std::vector<std::shared_ptr<nelement::SoundNode>> sound_nodes,
                           std::shared_ptr<nelement::Listener> listener)
            {
                mCamera = std::move(camera);
                mSoundNodes = std::move(sound_nodes);
                mListener = std::move(listener);
                mIsLoaded = true;
            }

            void clear_scene()
            {
                mCamera = nullptr;
                mSoundNodes.clear();
                mListener = nullptr;
                mSelectedNode.reset();
                mSelectedListener.reset();
            }

            void set_node_selected_callback(
                const std::function<void(const std::shared_ptr<nelement::SoundNode>&)>& callback)
            {
                mNodeSelectedCallback = callback;
            }

            void set_listener_selected_callback(
                const std::function<void(const std::shared_ptr<nelement::Listener>&)>& callback)
            {
                mListenerSelectedCallback = callback;
            }

            std::shared_ptr<nelement::Camera> get_camera() const
            {
                return mCamera;
            }

            const std::vector<std::shared_ptr<nelement::SoundNode>>& get_nodes() const
            {
                return mSoundNodes;
            }

            std::shared_ptr<nelement::Listener> get_listener() const
            {
                return mListener;
            }

            void add_sound_node()
            {
                auto node = std::make_shared<nelement::SoundNode>();
                node->init();
                node->set_engine(naudio::get_audio_context());
                node->set_name("node " + std::to_string(mSoundNodes.size() + 1));
                mSoundNodes.push_back(node);
            }

            void add_listener()
            {
                if (mListener) return; // engine only drives listener index 0

                mListener = std::make_shared<nelement::Listener>();
                mListener->init();
                mListener->set_engine(naudio::get_audio_context());
            }

            // Utils
            void draw_grid();

            // Scene Controls
            void on_mouse_move(double x, double y, nelement::EInputButton button);
            void on_mouse_wheel(double delta);

            std::shared_ptr<nelement::SoundNode> get_node(size_t index = 0)
            {
                if (index >= mSoundNodes.size()) return nullptr;
                return mSoundNodes[index];
            }
    };
} // namespace nui
