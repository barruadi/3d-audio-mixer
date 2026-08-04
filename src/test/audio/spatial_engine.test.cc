#include <gtest/gtest.h>

#include <glm/gtc/quaternion.hpp>
#include "audio/spatial_engine.hh"

// Unit tests for SpatialEngine that do not require audio files.
// All tests that call init() skip gracefully if the loopback device is
// unavailable (CI environments without OpenAL Soft support).

class SpatialEngineTest : public ::testing::Test
{
protected:
    naudio::SpatialEngine engine;

    bool tryInit(bool hrtf = false)
    {
        return engine.init(hrtf);
    }

    void TearDown() override
    {
        engine.shutdown();
    }
};

// ── Default state ─────────────────────────────────────────────────────────────

TEST(SpatialEngine_State, NotInitializedByDefault)
{
    naudio::SpatialEngine e;
    EXPECT_FALSE(e.is_initialized());
}

TEST(SpatialEngine_State, HRTFDisabledByDefault)
{
    naudio::SpatialEngine e;
    EXPECT_FALSE(e.is_hrtf_enabled());
}

TEST(SpatialEngine_State, ReverbDisabledByDefault)
{
    naudio::SpatialEngine e;
    EXPECT_FALSE(e.is_reverb_enabled());
}

// ── init / shutdown ───────────────────────────────────────────────────────────

TEST_F(SpatialEngineTest, InitReturnsTrueOnSuccess)
{
    if (!tryInit()) GTEST_SKIP() << "loopback device unavailable";
    EXPECT_TRUE(engine.is_initialized());
}

TEST_F(SpatialEngineTest, ShutdownResetsInitializedFlag)
{
    if (!tryInit()) GTEST_SKIP() << "loopback device unavailable";
    engine.shutdown();
    EXPECT_FALSE(engine.is_initialized());
}

TEST_F(SpatialEngineTest, DoubleShutdownDoesNotCrash)
{
    if (!tryInit()) GTEST_SKIP() << "loopback device unavailable";
    engine.shutdown();
    EXPECT_NO_THROW(engine.shutdown());
}

TEST_F(SpatialEngineTest, InitWithHRTFQueryable)
{
    if (!tryInit(true)) GTEST_SKIP() << "loopback device unavailable";
    // is_hrtf_enabled() reflects what the driver actually gave us — just check it doesn't crash
    EXPECT_NO_THROW(engine.is_hrtf_enabled());
}

// ── source management ─────────────────────────────────────────────────────────

TEST_F(SpatialEngineTest, AddSourceDoesNotCrash)
{
    naudio::SpatialSource src;
    src.filePath = "nonexistent.wav";
    src.position = { 1.0f, 0.0f, 0.0f };
    src.volume   = 0.5f;
    src.looping  = false;

    EXPECT_NO_THROW(engine.add_source(src));
}

TEST_F(SpatialEngineTest, ClearSourcesDoesNotCrash)
{
    naudio::SpatialSource src;
    src.filePath = "nonexistent.wav";
    engine.add_source(src);
    EXPECT_NO_THROW(engine.clear_sources());
}

// ── listener ──────────────────────────────────────────────────────────────────

TEST_F(SpatialEngineTest, SetListenerPositionBeforeInitDoesNotCrash)
{
    EXPECT_NO_THROW(engine.set_listener_position({ 1.0f, 2.0f, 3.0f }));
}

TEST_F(SpatialEngineTest, SetListenerPositionAfterInitDoesNotCrash)
{
    if (!tryInit()) GTEST_SKIP() << "loopback device unavailable";
    EXPECT_NO_THROW(engine.set_listener_position({ 0.0f, 1.0f, 0.0f }));
}

TEST_F(SpatialEngineTest, SetListenerOrientationDoesNotCrash)
{
    EXPECT_NO_THROW(engine.set_listener_orientation(
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 1.0f, 0.0f }));
}

// ── reverb ────────────────────────────────────────────────────────────────────

TEST_F(SpatialEngineTest, SetReverbNoneBeforeInitDoesNotCrash)
{
    EXPECT_NO_THROW(engine.set_reverb_preset(naudio::ReverbPreset::None));
}

TEST_F(SpatialEngineTest, SetReverbAfterInitDoesNotCrash)
{
    if (!tryInit()) GTEST_SKIP() << "loopback device unavailable";
    EXPECT_NO_THROW(engine.set_reverb_preset(naudio::ReverbPreset::SmallRoom));
    EXPECT_NO_THROW(engine.set_reverb_preset(naudio::ReverbPreset::None));
}

// ── render guard ──────────────────────────────────────────────────────────────

TEST_F(SpatialEngineTest, RenderToWavFailsIfNotInitialized)
{
    EXPECT_FALSE(engine.render_to_wav("/tmp/test_spatial_not_init.wav", 1.0f));
}

TEST_F(SpatialEngineTest, RenderToWavFailsIfDurationIsZero)
{
    if (!tryInit()) GTEST_SKIP() << "loopback device unavailable";
    EXPECT_FALSE(engine.render_to_wav("/tmp/test_spatial_zero_dur.wav", 0.0f));
}

TEST_F(SpatialEngineTest, RenderToWavFailsIfDurationIsNegative)
{
    if (!tryInit()) GTEST_SKIP() << "loopback device unavailable";
    EXPECT_FALSE(engine.render_to_wav("/tmp/test_spatial_neg_dur.wav", -1.0f));
}
