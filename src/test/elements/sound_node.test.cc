#include <gtest/gtest.h>

#include <glm/gtc/quaternion.hpp>
#include "elements/sound_node.hh"

// Tests operate on freshly constructed SoundNodes without calling init().
// init() and set_position() create/use an OpenGL vertex buffer — unsafe
// without a GL context. All tested methods guard on mSoundId == -1 and
// mVertexBuffer == nullptr, so they are safe in a headless environment.

TEST(SoundNode_Defaults, NameIsNode)
{
    nelement::SoundNode node;
    EXPECT_EQ(node.get_name(), "node");
}

TEST(SoundNode_Defaults, VolumeIsOne)
{
    nelement::SoundNode node;
    EXPECT_NEAR(node.get_volume(), 1.0f, 1e-6f);
}

TEST(SoundNode_Defaults, PanIsZero)
{
    nelement::SoundNode node;
    EXPECT_NEAR(node.get_pan(), 0.0f, 1e-6f);
}

TEST(SoundNode_Defaults, NotLooping)
{
    nelement::SoundNode node;
    EXPECT_FALSE(node.is_looping());
}

TEST(SoundNode_Defaults, NotLoaded)
{
    nelement::SoundNode node;
    EXPECT_FALSE(node.is_loaded());
}

TEST(SoundNode_Defaults, NotPlaying)
{
    nelement::SoundNode node;
    EXPECT_FALSE(node.is_playing());
}

TEST(SoundNode_Defaults, LoadNotFailed)
{
    nelement::SoundNode node;
    EXPECT_FALSE(node.has_load_failed());
}

TEST(SoundNode_Defaults, SoundIdIsMinusOne)
{
    nelement::SoundNode node;
    EXPECT_EQ(node.get_sound_id(), -1);
}

TEST(SoundNode_Defaults, CursorIsZeroWhenNotLoaded)
{
    nelement::SoundNode node;
    EXPECT_NEAR(node.get_cursor_seconds(), 0.0f, 1e-6f);
}

TEST(SoundNode_Defaults, LengthIsZeroWhenNotLoaded)
{
    nelement::SoundNode node;
    EXPECT_NEAR(node.get_length_seconds(), 0.0f, 1e-6f);
}

TEST(SoundNode_Defaults, FilePathIsEmpty)
{
    nelement::SoundNode node;
    EXPECT_TRUE(node.get_file_path().empty());
}

TEST(SoundNode_SetName, UpdatesName)
{
    nelement::SoundNode node;
    node.set_name("bass");
    EXPECT_EQ(node.get_name(), "bass");
}

TEST(SoundNode_SetFile, UpdatesFilePath)
{
    nelement::SoundNode node;
    node.set_file("/audio/kick.wav");
    EXPECT_EQ(node.get_file_path(), "/audio/kick.wav");
}

TEST(SoundNode_SetFile, ClearsLoadFailedFlag)
{
    nelement::SoundNode node;
    // Set a file — load failure state should be reset
    node.set_file("/audio/new.wav");
    EXPECT_FALSE(node.has_load_failed());
}

TEST(SoundNode_SetFile, SoundIdRemainsMinusOneWhenNoPreviousSound)
{
    nelement::SoundNode node;
    node.set_file("/audio/kick.wav");
    // No sound was loaded, so ID should still be -1
    EXPECT_EQ(node.get_sound_id(), -1);
}

TEST(SoundNode_SetLooping, ToggleLooping)
{
    nelement::SoundNode node;
    node.set_looping(true);
    EXPECT_TRUE(node.is_looping());

    node.set_looping(false);
    EXPECT_FALSE(node.is_looping());
}

TEST(SoundNode_SetVolume, UpdatesVolume)
{
    nelement::SoundNode node;
    node.set_volume(0.5f);
    EXPECT_NEAR(node.get_volume(), 0.5f, 1e-6f);
}

TEST(SoundNode_SetPan, UpdatesPan)
{
    nelement::SoundNode node;
    node.set_pan(-0.3f);
    EXPECT_NEAR(node.get_pan(), -0.3f, 1e-6f);
}

TEST(SoundNode_SetEngine, NullContextDoesNotCrash)
{
    nelement::SoundNode node;
    // Passing nullptr shared_ptr should not crash
    EXPECT_NO_THROW(node.set_engine(nullptr));
}
