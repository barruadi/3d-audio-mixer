#include <gtest/gtest.h>

// Include GLM quaternion support before camera/listener headers pull in pch.h
#include <glm/gtc/quaternion.hpp>
#include "elements/listener.hh"

// Tests operate on a freshly constructed Listener without calling init().
// init() creates an OpenGL vertex buffer — unsafe without a GL context.

TEST(Listener_Defaults, NameIsListener)
{
    nelement::Listener listener;
    EXPECT_EQ(listener.get_name(), "listener");
}

TEST(Listener_Defaults, PositionIsOrigin)
{
    nelement::Listener listener;
    glm::vec3 pos = listener.get_position();
    EXPECT_NEAR(pos.x, 0.0f, 1e-6f);
    EXPECT_NEAR(pos.y, 0.0f, 1e-6f);
    EXPECT_NEAR(pos.z, 0.0f, 1e-6f);
}

TEST(Listener_ApplyToEngine, DoesNotCrashWithoutContext)
{
    nelement::Listener listener;
    // No AudioContext attached — apply_to_engine() should be a no-op
    EXPECT_NO_THROW(listener.apply_to_engine());
}
