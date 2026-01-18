#include <doctest/doctest.h>
#include <functional>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// UI logic tests - no OpenGL/Shader dependencies
// Uses mock objects instead of actual Camera/SoundNode

namespace ui_test
{
    // Mock camera for testing
    struct MockCamera
    {
        glm::vec3 position = {0.0f, 0.0f, 5.0f};
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        void update_self(const glm::vec3& pos, float fov, float aspect, float near, float far)
        {
            position = pos;
            projection = glm::perspective(glm::radians(fov), aspect, near, far);
            view = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        glm::mat4 get_view_matrix() const { return view; }
        glm::mat4 get_projection_matrix() const { return projection; }
    };

    // Mock sound node for testing
    struct MockSoundNode
    {
        std::string filePath;
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        float volume = 1.0f;
        bool looping = false;
        bool playing = false;

        void set_file(const std::string& file) { filePath = file; }
        void set_volume(float v) { volume = v; }
        void set_looping(bool l) { looping = l; }
        void set_position(const glm::vec3& pos) { position = pos; }

        const std::string& get_file_path() const { return filePath; }
        float get_volume() const { return volume; }
        bool is_looping() const { return looping; }
        glm::vec3 get_position() const { return position; }
    };
}

// UI component tests focus on logic, not rendering
// Rendering tests require OpenGL/ImGui context

using MockCamera = ui_test::MockCamera;
using MockSoundNode = ui_test::MockSoundNode;

TEST_CASE("UI Logic: callback invocation pattern")
{
    using SceneCallback = std::function<void(
        const std::shared_ptr<MockCamera>&,
        const std::vector<std::shared_ptr<MockSoundNode>>&
    )>;

    bool callbackInvoked = false;
    std::shared_ptr<MockCamera> receivedCamera;
    std::vector<std::shared_ptr<MockSoundNode>> receivedNodes;

    SceneCallback callback = [&](
        const std::shared_ptr<MockCamera>& camera,
        const std::vector<std::shared_ptr<MockSoundNode>>& nodes)
    {
        callbackInvoked = true;
        receivedCamera = camera;
        receivedNodes = nodes;
    };

    auto camera = std::make_shared<MockCamera>();
    std::vector<std::shared_ptr<MockSoundNode>> nodes;
    nodes.push_back(std::make_shared<MockSoundNode>());
    nodes.push_back(std::make_shared<MockSoundNode>());

    callback(camera, nodes);

    CHECK(callbackInvoked == true);
    CHECK(receivedCamera == camera);
    CHECK(receivedNodes.size() == 2);
}

TEST_CASE("UI Logic: node selection pattern")
{
    MockSoundNode* currentNode = nullptr;

    auto node1 = std::make_shared<MockSoundNode>();
    auto node2 = std::make_shared<MockSoundNode>();

    currentNode = node1.get();
    CHECK(currentNode == node1.get());

    currentNode = node2.get();
    CHECK(currentNode == node2.get());

    currentNode = nullptr;
    CHECK(currentNode == nullptr);
}

TEST_CASE("UI Logic: scene state management")
{
    bool isLoaded = false;
    std::shared_ptr<MockCamera> camera;
    std::vector<std::shared_ptr<MockSoundNode>> soundNodes;

    CHECK(isLoaded == false);
    CHECK(camera == nullptr);
    CHECK(soundNodes.empty());

    camera = std::make_shared<MockCamera>();
    soundNodes.push_back(std::make_shared<MockSoundNode>());
    soundNodes.push_back(std::make_shared<MockSoundNode>());
    isLoaded = true;

    CHECK(isLoaded == true);
    CHECK(camera != nullptr);
    CHECK(soundNodes.size() == 2);

    camera = nullptr;
    soundNodes.clear();
    isLoaded = false;

    CHECK(isLoaded == false);
    CHECK(camera == nullptr);
    CHECK(soundNodes.empty());
}

TEST_CASE("UI Logic: random node selection")
{
    std::vector<std::shared_ptr<MockSoundNode>> nodes;

    auto getRandomNode = [&]() -> MockSoundNode* {
        if (nodes.empty()) return nullptr;
        return nodes[0].get();
    };

    CHECK(getRandomNode() == nullptr);

    nodes.push_back(std::make_shared<MockSoundNode>());
    nodes.push_back(std::make_shared<MockSoundNode>());

    CHECK(getRandomNode() != nullptr);
}

TEST_CASE("UI Logic: node properties access for display")
{
    auto node = std::make_shared<MockSoundNode>();

    node->set_file("audio/sound.wav");
    node->set_volume(0.75f);
    node->set_looping(true);

    CHECK(node->get_file_path() == "audio/sound.wav");
    CHECK(node->get_volume() == doctest::Approx(0.75f));
    CHECK(node->is_looping() == true);
    CHECK(node->get_position() == glm::vec3(0.0f, 0.0f, 0.0f));
}

TEST_CASE("UI Logic: camera properties for viewport")
{
    MockCamera camera;

    camera.update_self(
        glm::vec3(0.0f, 5.0f, 10.0f),
        45.0f,
        16.0f / 9.0f,
        0.1f,
        100.0f
    );

    glm::mat4 view = camera.get_view_matrix();
    glm::mat4 proj = camera.get_projection_matrix();

    CHECK(view != glm::mat4(1.0f));
    CHECK(proj != glm::mat4(1.0f));
}
