#include <doctest/doctest.h>
#include "audio/audio_system.hh"
#include "audio/audio_context.hh"

// Note: SoundNode tests are limited because SoundNode::set_position() and
// SoundNode::init() require OpenGL context (for vertex buffers).
// We test audio functionality directly through AudioSystem instead.

class SoundNodeAudioFixture
{
protected:
    std::shared_ptr<naudio::AudioContext> mContext;
    naudio::AudioSystem mSystem;

public:
    SoundNodeAudioFixture()
    {
        mContext = std::make_shared<naudio::AudioContext>();
        mContext->init();
        mSystem.attach_engine(mContext->get_engine());
    }

    ~SoundNodeAudioFixture()
    {
        mSystem.shutdown();
        mContext->shutdown();
    }
};

TEST_CASE_FIXTURE(SoundNodeAudioFixture, "SoundNode audio: load and play workflow")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    CHECK(mSystem.is_loaded(id) == true);
    CHECK(mSystem.is_playing(id) == false);

    mSystem.play(id);
    CHECK(mSystem.is_playing(id) == true);

    mSystem.stop(id);
    CHECK(mSystem.is_playing(id) == false);
}

TEST_CASE_FIXTURE(SoundNodeAudioFixture, "SoundNode audio: volume control")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    mSystem.set_volume(id, 0.5f);
    CHECK(mSystem.get_volume(id) == doctest::Approx(0.5f));

    mSystem.set_volume(id, 0.8f);
    CHECK(mSystem.get_volume(id) == doctest::Approx(0.8f));
}

TEST_CASE_FIXTURE(SoundNodeAudioFixture, "SoundNode audio: looping control")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    mSystem.set_looping(id, true);
    mSystem.set_looping(id, false);
    CHECK(true);  // No crash means success
}

TEST_CASE_FIXTURE(SoundNodeAudioFixture, "SoundNode audio: 3D position")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    mSystem.set_position(id, glm::vec3(5.0f, 2.0f, -3.0f));
    mSystem.set_position(id, glm::vec3(0.0f, 0.0f, 0.0f));
    CHECK(true);  // No crash means success
}

TEST_CASE_FIXTURE(SoundNodeAudioFixture, "SoundNode audio: pause and resume")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    mSystem.play(id);
    CHECK(mSystem.is_playing(id) == true);

    mSystem.pause(id);
    CHECK(mSystem.is_playing(id) == false);

    mSystem.resume(id);
    CHECK(mSystem.is_playing(id) == true);

    mSystem.stop(id);
}

TEST_CASE_FIXTURE(SoundNodeAudioFixture, "SoundNode audio: changing file unloads previous")
{
    int id1 = mSystem.load("tests/test_1.wav");
    REQUIRE(id1 >= 0);
    CHECK(mSystem.is_loaded(id1) == true);

    mSystem.unload(id1);
    CHECK(mSystem.is_loaded(id1) == false);

    int id2 = mSystem.load("tests/sound-sample-1.wav");
    REQUIRE(id2 >= 0);
    CHECK(mSystem.is_loaded(id2) == true);
}

TEST_CASE_FIXTURE(SoundNodeAudioFixture, "SoundNode audio: play with invalid file fails gracefully")
{
    int id = mSystem.load("nonexistent.wav");
    CHECK(id == -1);

    // Operations on invalid id should not crash
    mSystem.play(id);
    mSystem.stop(id);
    CHECK(mSystem.is_playing(id) == false);
}

TEST_CASE("SoundNode audio: operations without engine are safe")
{
    naudio::AudioSystem system;
    // No engine attached

    int id = system.load("tests/test_1.wav");
    CHECK(id == -1);

    system.play(0);
    system.stop(0);
    CHECK(true);  // No crash means success
}
