#include <doctest/doctest.h>
#include "audio/audio_context.hh"

TEST_CASE("AudioContext: init succeeds")
{
    naudio::AudioContext context;
    bool result = context.init();
    CHECK(result == true);
    context.shutdown();
}

TEST_CASE("AudioContext: get_engine returns valid pointer after init")
{
    naudio::AudioContext context;
    context.init();

    ma_engine* engine = context.get_engine();
    CHECK(engine != nullptr);

    context.shutdown();
}

TEST_CASE("AudioContext: set_listener_position does not crash")
{
    naudio::AudioContext context;
    context.init();

    // Various positions should not crash
    context.set_listener_position(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    context.set_listener_position(
        glm::vec3(100.0f, 50.0f, -25.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    context.set_listener_position(
        glm::vec3(-50.0f, -50.0f, -50.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    CHECK(true);
    context.shutdown();
}

TEST_CASE("AudioContext: multiple init/shutdown cycles work")
{
    naudio::AudioContext context;

    for (int i = 0; i < 3; i++)
    {
        bool result = context.init();
        CHECK(result == true);
        CHECK(context.get_engine() != nullptr);
        context.shutdown();
    }
}

TEST_CASE("AudioContext: can create multiple contexts")
{
    naudio::AudioContext context1;
    naudio::AudioContext context2;

    context1.init();
    context2.init();

    CHECK(context1.get_engine() != nullptr);
    CHECK(context2.get_engine() != nullptr);
    CHECK(context1.get_engine() != context2.get_engine());

    context1.shutdown();
    context2.shutdown();
}
