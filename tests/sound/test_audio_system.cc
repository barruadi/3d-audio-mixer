#include <doctest/doctest.h>
#include "audio/audio_system.hh"
#include "audio/audio_context.hh"

// Test fixture helper
class AudioSystemFixture
{
protected:
    naudio::AudioContext mContext;
    naudio::AudioSystem mSystem;

public:
    AudioSystemFixture()
    {
        mContext.init();
        mSystem.attach_engine(mContext.get_engine());
    }

    ~AudioSystemFixture()
    {
        mSystem.shutdown();
        mContext.shutdown();
    }
};

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: load returns valid ID for existing file")
{
    int id = mSystem.load("tests/test_1.wav");
    CHECK(id >= 0);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: load same file returns cached ID")
{
    int id1 = mSystem.load("tests/test_1.wav");
    int id2 = mSystem.load("tests/test_1.wav");
    CHECK(id1 == id2);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: load invalid file returns error")
{
    int id = mSystem.load("nonexistent_file.wav");
    CHECK(id == -1);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: is_loaded returns true after load")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);
    CHECK(mSystem.is_loaded(id) == true);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: is_loaded returns false for invalid ID")
{
    CHECK(mSystem.is_loaded(999) == false);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: unload removes sound")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    mSystem.unload(id);
    CHECK(mSystem.is_loaded(id) == false);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: play and stop change state")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    mSystem.play(id);
    CHECK(mSystem.is_playing(id) == true);

    mSystem.stop(id);
    CHECK(mSystem.is_playing(id) == false);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: pause and resume work correctly")
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

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: set_volume applies correctly")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    mSystem.set_volume(id, 0.5f);
    CHECK(mSystem.get_volume(id) == doctest::Approx(0.5f));

    mSystem.set_volume(id, 1.0f);
    CHECK(mSystem.get_volume(id) == doctest::Approx(1.0f));
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: set_position does not crash")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    // Should not throw or crash
    mSystem.set_position(id, glm::vec3(5.0f, 2.0f, -3.0f));
    mSystem.set_position(id, glm::vec3(0.0f, 0.0f, 0.0f));
    mSystem.set_position(id, glm::vec3(-100.0f, 100.0f, 50.0f));
    CHECK(true);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: set_looping does not crash")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    mSystem.set_looping(id, true);
    mSystem.set_looping(id, false);
    CHECK(true);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: set_pitch does not crash")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    mSystem.set_pitch(id, 0.5f);
    mSystem.set_pitch(id, 1.0f);
    mSystem.set_pitch(id, 2.0f);
    CHECK(true);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: distance settings do not crash")
{
    int id = mSystem.load("tests/test_1.wav");
    REQUIRE(id >= 0);

    mSystem.set_min_distance(id, 1.0f);
    mSystem.set_max_distance(id, 100.0f);
    mSystem.set_attenuation(id, ma_attenuation_model_inverse);
    mSystem.set_attenuation(id, ma_attenuation_model_linear);
    CHECK(true);
}

TEST_CASE_FIXTURE(AudioSystemFixture, "AudioSystem: unload_all clears all sounds")
{
    int id1 = mSystem.load("tests/test_1.wav");
    int id2 = mSystem.load("tests/sound-sample-1.wav");
    REQUIRE(id1 >= 0);
    REQUIRE(id2 >= 0);

    mSystem.unload_all();

    CHECK(mSystem.is_loaded(id1) == false);
    CHECK(mSystem.is_loaded(id2) == false);
}

TEST_CASE("AudioSystem: attach_engine with null does not crash")
{
    naudio::AudioSystem system;
    system.attach_engine(nullptr);

    int id = system.load("tests/test_1.wav");
    CHECK(id == -1);
}

TEST_CASE("AudioSystem: operations on invalid IDs do not crash")
{
    naudio::AudioContext context;
    naudio::AudioSystem system;
    context.init();
    system.attach_engine(context.get_engine());

    // None of these should crash
    system.play(999);
    system.stop(999);
    system.pause(999);
    system.resume(999);
    system.set_volume(999, 1.0f);
    system.set_position(999, glm::vec3(0.0f));
    system.unload(999);

    CHECK(system.is_playing(999) == false);
    CHECK(system.is_loaded(999) == false);

    system.shutdown();
    context.shutdown();
}
