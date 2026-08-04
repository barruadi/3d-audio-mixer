#include <gtest/gtest.h>

#include <glm/gtc/quaternion.hpp>
#include "audio/spatial_engine.hh"
#include "elements/sound_node.hh"
#include "elements/listener.hh"

#include <fstream>
#include <cstdio>
#include <string>

#ifndef TEST_AUDIO_DIR
#define TEST_AUDIO_DIR "tests"
#endif

static const std::string kTestWav     = std::string(TEST_AUDIO_DIR) + "/sound-sample-1.wav";
static const std::string kRenderOut   = "/tmp/3dam_render_test.wav";

// Integration tests: require the loopback device + real audio files.
// Skipped automatically if the loopback device is unavailable.

class SpatialEngineIntegration : public ::testing::Test
{
protected:
    naudio::SpatialEngine engine;
    bool mReady = false;

    void SetUp() override
    {
        mReady = engine.init(false); // no HRTF to keep it simple for CI
        if (!mReady) return;

        // Check the test audio file exists.
        std::ifstream f(kTestWav);
        if (!f.is_open())
        {
            mReady = false;
        }
    }

    void TearDown() override
    {
        engine.shutdown();
        std::remove(kRenderOut.c_str());
    }
};

// ── render with no sources ────────────────────────────────────────────────────

TEST_F(SpatialEngineIntegration, RenderSilenceProducesWavFile)
{
    if (!mReady) GTEST_SKIP() << "loopback device or test audio unavailable";

    ASSERT_TRUE(engine.render_to_wav(kRenderOut, 0.5f));

    std::ifstream f(kRenderOut, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(f.is_open());
    // WAV header alone is 44 bytes; silence render should produce > that
    EXPECT_GT(f.tellg(), (std::streamsize)44);
}

// ── render with one positioned source ─────────────────────────────────────────

TEST_F(SpatialEngineIntegration, RenderWithOneSourceProducesNonEmptyFile)
{
    if (!mReady) GTEST_SKIP() << "loopback device or test audio unavailable";

    naudio::SpatialSource src;
    src.filePath = kTestWav;
    src.position = { 2.0f, 0.0f, 0.0f };
    src.volume   = 1.0f;
    src.looping  = false;
    engine.add_source(src);

    ASSERT_TRUE(engine.render_to_wav(kRenderOut, 1.0f));

    std::ifstream f(kRenderOut, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(f.is_open());
    EXPECT_GT(f.tellg(), (std::streamsize)44);
}

// ── SoundNode.to_spatial_source integration ───────────────────────────────────

TEST_F(SpatialEngineIntegration, SoundNodeToSpatialSourceMatchesNodeProperties)
{
    if (!mReady) GTEST_SKIP() << "loopback device or test audio unavailable";

    nelement::SoundNode node;
    node.set_name("test_node");
    node.set_file(kTestWav);
    node.set_volume(0.6f);
    node.set_pan(0.0f);
    node.set_looping(true);

    naudio::SpatialSource src = node.to_spatial_source();

    EXPECT_EQ(src.filePath, kTestWav);
    EXPECT_NEAR(src.volume, 0.6f, 1e-5f);
    EXPECT_EQ(src.looping, true);
    // position is (0,0,0) — set_position not called to avoid GL crash
    EXPECT_NEAR(src.position.x, 0.0f, 1e-5f);
    EXPECT_NEAR(src.position.y, 0.0f, 1e-5f);
    EXPECT_NEAR(src.position.z, 0.0f, 1e-5f);
}

TEST_F(SpatialEngineIntegration, RenderFromSoundNodeSource)
{
    if (!mReady) GTEST_SKIP() << "loopback device or test audio unavailable";

    nelement::SoundNode node;
    node.set_file(kTestWav);
    node.set_volume(0.8f);

    engine.add_source(node.to_spatial_source());
    EXPECT_TRUE(engine.render_to_wav(kRenderOut, 1.0f));
}

// ── Listener integration ──────────────────────────────────────────────────────

TEST(SpatialEngine_ListenerIntegration, OrientationAtIsNegativeZ)
{
    nelement::Listener listener;
    glm::vec3 at = listener.get_orientation_at();
    EXPECT_NEAR(at.x,  0.0f, 1e-5f);
    EXPECT_NEAR(at.y,  0.0f, 1e-5f);
    EXPECT_NEAR(at.z, -1.0f, 1e-5f);
}

TEST(SpatialEngine_ListenerIntegration, OrientationUpIsPositiveY)
{
    nelement::Listener listener;
    glm::vec3 up = listener.get_orientation_up();
    EXPECT_NEAR(up.x, 0.0f, 1e-5f);
    EXPECT_NEAR(up.y, 1.0f, 1e-5f);
    EXPECT_NEAR(up.z, 0.0f, 1e-5f);
}

TEST_F(SpatialEngineIntegration, ApplyListenerFromListenerElement)
{
    if (!mReady) GTEST_SKIP() << "loopback device or test audio unavailable";

    nelement::Listener listener;
    // Listener starts at origin with default orientation
    engine.set_listener_position(listener.get_position());
    engine.set_listener_orientation(listener.get_orientation_at(),
                                    listener.get_orientation_up());

    naudio::SpatialSource src;
    src.filePath = kTestWav;
    engine.add_source(src);

    EXPECT_TRUE(engine.render_to_wav(kRenderOut, 0.5f));
}

// ── reverb integration ────────────────────────────────────────────────────────

TEST_F(SpatialEngineIntegration, RenderWithReverbDoesNotCrash)
{
    if (!mReady) GTEST_SKIP() << "loopback device or test audio unavailable";

    engine.set_reverb_preset(naudio::ReverbPreset::SmallRoom);
    naudio::SpatialSource src;
    src.filePath = kTestWav;
    engine.add_source(src);

    // Whether reverb is enabled depends on EFX availability — just must not crash
    EXPECT_NO_THROW(engine.render_to_wav(kRenderOut, 0.5f));
}
