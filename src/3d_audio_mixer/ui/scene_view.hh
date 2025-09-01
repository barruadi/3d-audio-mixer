#pragma once

#include "elements/sound_node.hh"
#include "elements/camera.hh"
#include <vector>
#include <nlohmann/json.hpp>

namespace nui
{
    class SceneView
    {
        private:
            bool mIsLoaded;
            std::unique_ptr<nelement::Camera> mCamera;
            std::vector<std::unique_ptr<nelement::SoundNode>> mSoundNodes;

        public:
            SceneView():
                mIsLoaded(false),
                mCamera(nullptr),
                mSoundNodes()
            {
                mCamera = std::make_unique<nelement::Camera>();
            }

            ~SceneView()
            {
                // Clean up resources
            }

            void render();

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
    };
} // namespace nui
