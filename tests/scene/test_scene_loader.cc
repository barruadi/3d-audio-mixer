#include <doctest/doctest.h>
#include <nlohmann/json.hpp>

// Note: Full SceneLoader tests with SoundNode require OpenGL context
// These tests focus on JSON parsing logic and camera loading only

#include "elements/camera.hh"

// Helper class to test JSON parsing logic without OpenGL dependencies
namespace scene_test
{
    bool read_vec3(const nlohmann::json& arr, float& x, float& y, float& z)
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

    struct NodeData
    {
        std::string type;
        glm::vec3 position;
        std::string file;
        float volume;
        float pan;
    };

    bool parse_camera(const nlohmann::json& data,
                      glm::vec3& position, float& fov, float& aspect, float& near, float& far)
    {
        if (!data.contains("camera") || !data["camera"].is_object()) return false;

        const auto& jcam = data["camera"];

        float px = 0, py = 0, pz = 5;
        read_vec3(jcam.value("position", nlohmann::json::array({0.0f, 0.0f, 3.0f})), px, py, pz);

        position = glm::vec3(px, py, pz);
        fov = jcam.value("fov", 45.0f);
        aspect = jcam.value("aspect", 16.0f / 9.0f);
        near = jcam.value("near", 0.1f);
        far = jcam.value("far", 100.0f);

        return true;
    }

    std::vector<NodeData> parse_nodes(const nlohmann::json& data)
    {
        std::vector<NodeData> nodes;

        if (!data.contains("nodes") || !data["nodes"].is_array()) return nodes;

        for (const auto& n : data["nodes"])
        {
            if (!n.is_object()) continue;

            NodeData node;
            node.type = n.contains("type") ? n["type"].get<std::string>() : "";

            if (node.type != "audio") continue;

            float x = 0, y = 0, z = 0;
            if (n.contains("position") && n["position"].is_array())
            {
                read_vec3(n["position"], x, y, z);
            }
            node.position = glm::vec3(x, y, z);

            node.file = n.contains("file") ? n["file"].get<std::string>() : "";
            if (node.file.empty()) continue;

            node.volume = 1.0f;
            node.pan = 0.0f;
            if (n.contains("properties") && n["properties"].is_object())
            {
                const auto& props = n["properties"];
                if (props.contains("volume")) node.volume = props["volume"].get<float>();
                if (props.contains("pan")) node.pan = props["pan"].get<float>();
            }

            nodes.push_back(node);
        }

        return nodes;
    }
}

TEST_CASE("Scene: parse camera from valid JSON")
{
    nlohmann::json data = {
        {"camera", {
            {"position", {1.0f, 2.0f, 3.0f}},
            {"fov", 60.0f},
            {"aspect", 1.5f},
            {"near", 0.5f},
            {"far", 500.0f}
        }}
    };

    glm::vec3 position;
    float fov, aspect, near, far;
    bool result = scene_test::parse_camera(data, position, fov, aspect, near, far);

    CHECK(result == true);
    CHECK(position == glm::vec3(1.0f, 2.0f, 3.0f));
    CHECK(fov == doctest::Approx(60.0f));
    CHECK(aspect == doctest::Approx(1.5f));
    CHECK(near == doctest::Approx(0.5f));
    CHECK(far == doctest::Approx(500.0f));
}

TEST_CASE("Scene: parse camera with default values")
{
    nlohmann::json data = {
        {"camera", nlohmann::json::object()}
    };

    glm::vec3 position;
    float fov, aspect, near, far;
    bool result = scene_test::parse_camera(data, position, fov, aspect, near, far);

    CHECK(result == true);
    CHECK(fov == doctest::Approx(45.0f));
    CHECK(near == doctest::Approx(0.1f));
    CHECK(far == doctest::Approx(100.0f));
}

TEST_CASE("Scene: parse camera fails without camera object")
{
    nlohmann::json data = {
        {"other_key", "value"}
    };

    glm::vec3 position;
    float fov, aspect, near, far;
    bool result = scene_test::parse_camera(data, position, fov, aspect, near, far);

    CHECK(result == false);
}

TEST_CASE("Scene: parse nodes from valid JSON")
{
    nlohmann::json data = {
        {"nodes", {
            {
                {"type", "audio"},
                {"position", {1.0f, 2.0f, 3.0f}},
                {"file", "tests/test_1.wav"},
                {"properties", {
                    {"volume", 0.8f},
                    {"pan", -0.5f}
                }}
            },
            {
                {"type", "audio"},
                {"position", {-1.0f, 0.0f, 5.0f}},
                {"file", "tests/sound-sample-1.wav"},
                {"properties", {
                    {"volume", 0.5f},
                    {"pan", 0.0f}
                }}
            }
        }}
    };

    auto nodes = scene_test::parse_nodes(data);

    CHECK(nodes.size() == 2);
}

TEST_CASE("Scene: node has correct position")
{
    nlohmann::json data = {
        {"nodes", {
            {
                {"type", "audio"},
                {"position", {5.0f, 10.0f, -3.0f}},
                {"file", "test.wav"},
                {"properties", {}}
            }
        }}
    };

    auto nodes = scene_test::parse_nodes(data);

    REQUIRE(nodes.size() == 1);
    CHECK(nodes[0].position == glm::vec3(5.0f, 10.0f, -3.0f));
}

TEST_CASE("Scene: node has correct file path")
{
    nlohmann::json data = {
        {"nodes", {
            {
                {"type", "audio"},
                {"position", {0.0f, 0.0f, 0.0f}},
                {"file", "path/to/sound.wav"},
                {"properties", {}}
            }
        }}
    };

    auto nodes = scene_test::parse_nodes(data);

    REQUIRE(nodes.size() == 1);
    CHECK(nodes[0].file == "path/to/sound.wav");
}

TEST_CASE("Scene: node has correct volume")
{
    nlohmann::json data = {
        {"nodes", {
            {
                {"type", "audio"},
                {"position", {0.0f, 0.0f, 0.0f}},
                {"file", "test.wav"},
                {"properties", {
                    {"volume", 0.75f}
                }}
            }
        }}
    };

    auto nodes = scene_test::parse_nodes(data);

    REQUIRE(nodes.size() == 1);
    CHECK(nodes[0].volume == doctest::Approx(0.75f));
}

TEST_CASE("Scene: skips non-audio nodes")
{
    nlohmann::json data = {
        {"nodes", {
            {
                {"type", "light"},
                {"position", {0.0f, 5.0f, 0.0f}}
            },
            {
                {"type", "audio"},
                {"position", {1.0f, 0.0f, 0.0f}},
                {"file", "sound.wav"},
                {"properties", {}}
            },
            {
                {"type", "mesh"},
                {"position", {0.0f, 0.0f, 0.0f}}
            }
        }}
    };

    auto nodes = scene_test::parse_nodes(data);

    CHECK(nodes.size() == 1);
}

TEST_CASE("Scene: skips nodes without file")
{
    nlohmann::json data = {
        {"nodes", {
            {
                {"type", "audio"},
                {"position", {0.0f, 0.0f, 0.0f}},
                {"properties", {}}
            },
            {
                {"type", "audio"},
                {"position", {0.0f, 0.0f, 0.0f}},
                {"file", ""},
                {"properties", {}}
            }
        }}
    };

    auto nodes = scene_test::parse_nodes(data);

    CHECK(nodes.size() == 0);
}

TEST_CASE("Scene: returns empty for empty nodes array")
{
    nlohmann::json data = {
        {"nodes", nlohmann::json::array()}
    };

    auto nodes = scene_test::parse_nodes(data);

    CHECK(nodes.empty());
}

TEST_CASE("Scene: returns empty without nodes key")
{
    nlohmann::json data = {
        {"camera", {}}
    };

    auto nodes = scene_test::parse_nodes(data);

    CHECK(nodes.empty());
}

TEST_CASE("Scene: default volume is 1.0")
{
    nlohmann::json data = {
        {"nodes", {
            {
                {"type", "audio"},
                {"position", {0.0f, 0.0f, 0.0f}},
                {"file", "sound.wav"}
            }
        }}
    };

    auto nodes = scene_test::parse_nodes(data);

    REQUIRE(nodes.size() == 1);
    CHECK(nodes[0].volume == doctest::Approx(1.0f));
}

TEST_CASE("Scene: read_vec3 helper")
{
    float x, y, z;

    // Valid array
    CHECK(scene_test::read_vec3(nlohmann::json::array({1.0f, 2.0f, 3.0f}), x, y, z) == true);
    CHECK(x == doctest::Approx(1.0f));
    CHECK(y == doctest::Approx(2.0f));
    CHECK(z == doctest::Approx(3.0f));

    // Array too short
    CHECK(scene_test::read_vec3(nlohmann::json::array({1.0f, 2.0f}), x, y, z) == false);

    // Not an array
    CHECK(scene_test::read_vec3(nlohmann::json(42), x, y, z) == false);
}
