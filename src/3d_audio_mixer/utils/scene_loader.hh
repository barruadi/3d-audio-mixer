#pragma once

#include "elements/sound_node.hh"
#include "elements/camera.hh"
#include <nlohmann/json.hpp>
#include <vector>

namespace nutils
{
    class SceneLoader
    {
        private:
            nlohmann::json data;

            static bool read_vec3(const nlohmann::json& arr, float& x, float& y, float& z)
            {
                if (!arr.is_array() || arr.size() < 3) return false;

                auto getf = [](const nlohmann::json& j, size_t i, float def) -> float {
                    if (i < j.size() && j[i].is_number()) return j[i].get<float>();
                    return def;
                };

                x = getf(arr, 0, 0.0f);
                y = getf(arr, 1, 0.0f);
                z = getf(arr, 2, 0.0f);
                return true;
            }

        public:
            void set_data(const nlohmann::json& json_data)
            {
                data = json_data;
            }

            bool load_camera(std::shared_ptr<nelement::Camera>* camera)
            {
                if (!data.contains("camera") || !data["camera"].is_object()) return false;

                const auto& jcam = data["camera"];

                float px = 0, py = 0, pz = 5;
                read_vec3(jcam.value("position", nlohmann::json::array({0.0f, 0.0f, 5.0f})),
                    px, py, pz
                );

                float fov    = jcam.value("fov", 45.0f);
                float aspect = jcam.value("aspect", 16.0f / 9.0f);
                float near   = jcam.value("near", 0.1f);
                float far    = jcam.value("far", 100.0f);

                (*camera)->update(glm::vec3(px, py, pz), fov, aspect, near, far);

                return true;
            }

            bool load_sound_nodes(std::vector<std::shared_ptr<nelement::SoundNode>>* sound_nodes)
            {
                sound_nodes->clear();
                
                if (!data.contains("sound_node") || !data["sound_node"].is_object()) return false;
                const auto& jnodes = data.value("nodes", nlohmann::json::array());

                for (const auto& n : jnodes)
                {
                    if (!n.is_object()) continue;
                    if (n.value("type", std::string{}) != "audio") continue;

                    float x = 0, y = 0, z = 0;
                    read_vec3(n.value("position", nlohmann::json::array()), x, y, z);
                    
                    std::string file = n.value("file", std::string{});
                    if (file.empty()) continue;

                    const auto& n_props = n.value("properties", nlohmann::json::object());
                    float volume = n_props.value("volume", 1.0f);
                    float pan    = n_props.value("pan", 0.0f);

                    auto sn = std::make_shared<nelement::SoundNode>();

                    // set values
                    sn->update(file, glm::vec3(x, y, z), volume, pan);

                    sound_nodes->push_back(sn);
                }
            }
    };
} // namespace nutils
