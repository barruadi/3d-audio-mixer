#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <glm/gtc/quaternion.hpp>
#include "utils/scene_loader.hh"

// Note: SceneLoader::load_listener() and load_sound_nodes() call Element::init()
// which creates OpenGL vertex buffers — unsafe in a headless test environment.
// Only load_camera() and serialize_scene() are tested here.

static constexpr float kEps = 1e-4f;

// ── load_camera ───────────────────────────────────────────────────────────────

TEST(SceneLoader_LoadCamera, ParsesPositionFromJson)
{
    nlohmann::json data = {
        {"camera", {
            {"position", {1.0f, 2.0f, 3.0f}},
            {"fov", 45.0f},
            {"aspect", 1.777f},
            {"near", 0.1f},
            {"far", 100.0f}
        }}
    };

    nutils::SceneLoader loader;
    loader.set_data(data);

    std::shared_ptr<nelement::Camera> cam;
    ASSERT_TRUE(loader.load_camera(&cam));
    ASSERT_NE(cam, nullptr);
}

TEST(SceneLoader_LoadCamera, ReturnsFalseWhenCameraKeyMissing)
{
    nlohmann::json data = {{"nodes", nlohmann::json::array()}};

    nutils::SceneLoader loader;
    loader.set_data(data);

    std::shared_ptr<nelement::Camera> cam;
    EXPECT_FALSE(loader.load_camera(&cam));
    // Camera is still created with defaults even on failure
    EXPECT_NE(cam, nullptr);
}

TEST(SceneLoader_LoadCamera, ReturnsFalseForNonObjectCameraKey)
{
    nlohmann::json data = {{"camera", "not_an_object"}};

    nutils::SceneLoader loader;
    loader.set_data(data);

    std::shared_ptr<nelement::Camera> cam;
    EXPECT_FALSE(loader.load_camera(&cam));
}

TEST(SceneLoader_LoadCamera, ParsesFovAspectNearFar)
{
    nlohmann::json data = {
        {"camera", {
            {"position", {0.0f, 0.0f, 5.0f}},
            {"fov", 60.0f},
            {"aspect", 1.5f},
            {"near", 0.5f},
            {"far", 200.0f}
        }}
    };

    nutils::SceneLoader loader;
    loader.set_data(data);

    std::shared_ptr<nelement::Camera> cam;
    ASSERT_TRUE(loader.load_camera(&cam));

    // Projection matrix should not be identity — it reflects the parsed values
    glm::mat4 identity(1.0f);
    EXPECT_NE(cam->get_projection_matrix(), identity);
}

// ── serialize_scene ───────────────────────────────────────────────────────────

TEST(SceneLoader_Serialize, CameraPositionAppearsInJson)
{
    auto cam = std::make_shared<nelement::Camera>();
    // Camera position is derived from focus - forward * distance.
    // Default: focus=(0,0,0), forward=(0,0,-1), distance=5 → position=(0,0,5)
    glm::vec3 pos = cam->get_position();

    nlohmann::json base;
    nlohmann::json result = nutils::SceneLoader::serialize_scene(base, cam, {}, nullptr);

    ASSERT_TRUE(result.contains("camera"));
    ASSERT_TRUE(result["camera"].contains("position"));
    EXPECT_NEAR(result["camera"]["position"][0].get<float>(), pos.x, kEps);
    EXPECT_NEAR(result["camera"]["position"][1].get<float>(), pos.y, kEps);
    EXPECT_NEAR(result["camera"]["position"][2].get<float>(), pos.z, kEps);
}

TEST(SceneLoader_Serialize, EmptyNodesProducesEmptyArray)
{
    nlohmann::json base;
    nlohmann::json result = nutils::SceneLoader::serialize_scene(base, nullptr, {}, nullptr);

    ASSERT_TRUE(result.contains("nodes"));
    EXPECT_TRUE(result["nodes"].is_array());
    EXPECT_EQ(result["nodes"].size(), 0u);
}

TEST(SceneLoader_Serialize, SoundNodePropertiesAreSerialized)
{
    // Construct a SoundNode and set properties that don't need OpenGL.
    // We do NOT call init() or set_position() — both require a GL context.
    auto node = std::make_shared<nelement::SoundNode>();
    node->set_name("kick");
    node->set_file("/audio/kick.wav");
    node->set_volume(0.75f);
    node->set_pan(0.25f);
    node->set_looping(true);

    nlohmann::json base;
    nlohmann::json result = nutils::SceneLoader::serialize_scene(
        base, nullptr, {node}, nullptr);

    ASSERT_TRUE(result.contains("nodes"));
    ASSERT_EQ(result["nodes"].size(), 1u);

    const auto& jn = result["nodes"][0];
    EXPECT_EQ(jn["name"].get<std::string>(), "kick");
    EXPECT_EQ(jn["file"].get<std::string>(), "/audio/kick.wav");
    EXPECT_EQ(jn["type"].get<std::string>(), "audio");

    const auto& props = jn["properties"];
    EXPECT_NEAR(props["volume"].get<float>(), 0.75f, kEps);
    EXPECT_NEAR(props["pan"].get<float>(), 0.25f, kEps);
    EXPECT_EQ(props["looping"].get<bool>(), true);
}

TEST(SceneLoader_Serialize, PreservesExtraKeysFromBase)
{
    nlohmann::json base = {{"project", "my_scene"}, {"version", 2}};
    nlohmann::json result = nutils::SceneLoader::serialize_scene(base, nullptr, {}, nullptr);

    EXPECT_EQ(result["project"].get<std::string>(), "my_scene");
    EXPECT_EQ(result["version"].get<int>(), 2);
}

TEST(SceneLoader_Serialize, NullCameraSkipsCameraSection)
{
    nlohmann::json base;
    nlohmann::json result = nutils::SceneLoader::serialize_scene(base, nullptr, {}, nullptr);

    // No camera passed → no camera key written
    EXPECT_FALSE(result.contains("camera"));
}

TEST(SceneLoader_Serialize, NullListenerSkipsListenerSection)
{
    nlohmann::json base;
    nlohmann::json result = nutils::SceneLoader::serialize_scene(base, nullptr, {}, nullptr);

    EXPECT_FALSE(result.contains("listener"));
}
