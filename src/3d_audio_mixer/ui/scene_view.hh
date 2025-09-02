#pragma once

#include "elements/sound_node.hh"
#include "elements/camera.hh"
#include "render/opengl_buffer_manager.hh"
#include "shader/shader.hh"
#include <vector>
#include <nlohmann/json.hpp>

namespace nui
{
    class SceneView
    {
        private:
            bool mIsLoaded;

            // Scene objects
            std::unique_ptr<nelement::Camera> mCamera;
            std::vector<std::unique_ptr<nelement::SoundNode>> mSoundNodes;
            
            // framebuffer
            std::unique_ptr<nrender::OpenGL_FrameBuffer> mFrameBuffer;

            // shader
            std::unique_ptr<nshader::Shader> mShader;

        public:
            SceneView():
                mIsLoaded(false),
                mCamera(nullptr),
                mSoundNodes()
            {
                mCamera = std::make_unique<nelement::Camera>();
                mFrameBuffer = std::make_unique<nrender::OpenGL_FrameBuffer>();
                mFrameBuffer->create_buffers(800, 600);

                mShader = std::make_unique<nshader::Shader>();
                mShader->load("shaders/node_vert.shader", "shaders/node_frag.shader");
            }

            ~SceneView()
            {
                // Clean up resources
            }

            void render();

            void resize(int32_t width, int32_t height);

            // [TODO]: make universal in case of other format
            bool load_scene(nlohmann::json data);

            void set_scene(std::unique_ptr<nelement::Camera> camera,
                           std::vector<std::unique_ptr<nelement::SoundNode>> sound_nodes)
            {
                mCamera = std::move(camera);
                mSoundNodes = std::move(sound_nodes);
                mIsLoaded = true;
            }

            void clear_scene()
            {
                mCamera = nullptr;
                mSoundNodes.clear();
            }

            // Scene Controls
            void on_mouse_move(double x, double y, nelement::EInputButton button);
            void on_mouse_wheel(double delta);
    };
} // namespace nui
