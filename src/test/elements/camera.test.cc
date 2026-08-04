#include <gtest/gtest.h>

// Quaternion support must come before camera.hh pulls in pch.h (which
// only includes glm.hpp and matrix_transform, not quaternion).
#include <glm/gtc/quaternion.hpp>
#include "elements/camera.hh"

static constexpr float kEps = 1e-4f;

// ── Default state ─────────────────────────────────────────────────────────────

TEST(Camera_Defaults, ViewMatrixIsNotIdentity)
{
    nelement::Camera cam;
    // Camera starts at distance 5 from origin — view is not identity
    glm::mat4 identity(1.0f);
    EXPECT_NE(cam.get_view_matrix(), identity);
}

TEST(Camera_Defaults, ProjectionIsNotIdentity)
{
    nelement::Camera cam;
    glm::mat4 identity(1.0f);
    EXPECT_NE(cam.get_projection_matrix(), identity);
}

// ── Basis vectors ─────────────────────────────────────────────────────────────

TEST(Camera_BasisVectors, ForwardIsUnitLength)
{
    nelement::Camera cam;
    EXPECT_NEAR(glm::length(cam.get_forward()), 1.0f, kEps);
}

TEST(Camera_BasisVectors, UpIsUnitLength)
{
    nelement::Camera cam;
    EXPECT_NEAR(glm::length(cam.get_up()), 1.0f, kEps);
}

TEST(Camera_BasisVectors, RightIsUnitLength)
{
    nelement::Camera cam;
    EXPECT_NEAR(glm::length(cam.get_right()), 1.0f, kEps);
}

TEST(Camera_BasisVectors, DefaultForwardPointsNegativeZ)
{
    nelement::Camera cam;
    glm::vec3 fwd = cam.get_forward();
    EXPECT_NEAR(fwd.x, 0.0f, kEps);
    EXPECT_NEAR(fwd.y, 0.0f, kEps);
    EXPECT_NEAR(fwd.z, -1.0f, kEps);
}

TEST(Camera_BasisVectors, DefaultUpPointsPositiveY)
{
    nelement::Camera cam;
    glm::vec3 up = cam.get_up();
    EXPECT_NEAR(up.x, 0.0f, kEps);
    EXPECT_NEAR(up.y, 1.0f, kEps);
    EXPECT_NEAR(up.z, 0.0f, kEps);
}

TEST(Camera_BasisVectors, DefaultRightPointsPositiveX)
{
    nelement::Camera cam;
    glm::vec3 right = cam.get_right();
    EXPECT_NEAR(right.x, 1.0f, kEps);
    EXPECT_NEAR(right.y, 0.0f, kEps);
    EXPECT_NEAR(right.z, 0.0f, kEps);
}

TEST(Camera_BasisVectors, BasisIsOrthonormal)
{
    nelement::Camera cam;
    glm::vec3 f = cam.get_forward();
    glm::vec3 u = cam.get_up();
    glm::vec3 r = cam.get_right();

    EXPECT_NEAR(glm::dot(f, u), 0.0f, kEps);
    EXPECT_NEAR(glm::dot(f, r), 0.0f, kEps);
    EXPECT_NEAR(glm::dot(u, r), 0.0f, kEps);
}

// ── Direction (quaternion) ────────────────────────────────────────────────────

TEST(Camera_GetDirection, DefaultIsIdentityQuaternion)
{
    nelement::Camera cam;
    glm::quat q = cam.get_direction();
    EXPECT_NEAR(q.w, 1.0f, kEps);
    EXPECT_NEAR(q.x, 0.0f, kEps);
    EXPECT_NEAR(q.y, 0.0f, kEps);
    EXPECT_NEAR(q.z, 0.0f, kEps);
}

// ── Mouse wheel (zoom) ────────────────────────────────────────────────────────

TEST(Camera_MouseWheel, ScrollInMovesPositionCloserToFocus)
{
    nelement::Camera cam;
    glm::vec3 before = cam.get_position();
    // Delta=1 reduces distance from 5→3; camera moves closer to focus
    cam.on_mouse_wheel(1.0);
    glm::vec3 after = cam.get_position();
    EXPECT_LT(glm::length(after), glm::length(before));
}

TEST(Camera_MouseWheel, ScrollOutMovesPositionFartherFromFocus)
{
    nelement::Camera cam;
    glm::vec3 before = cam.get_position();
    cam.on_mouse_wheel(-5.0);
    glm::vec3 after = cam.get_position();
    EXPECT_GT(glm::length(after), glm::length(before));
}

TEST(Camera_MouseWheel, ViewMatrixChangesAfterScroll)
{
    nelement::Camera cam;
    glm::mat4 before = cam.get_view_matrix();
    cam.on_mouse_wheel(1.0);
    EXPECT_NE(cam.get_view_matrix(), before);
}

// ── Mouse move (orbit / pan) ──────────────────────────────────────────────────

TEST(Camera_MouseMove, RightButtonDragChangesViewMatrix)
{
    nelement::Camera cam;
    glm::mat4 before = cam.get_view_matrix();
    // Simulate first move to set mCurrentPos2d, then a drag
    cam.on_mouse_move(0.0, 0.0, nelement::EInputButton::None);
    cam.on_mouse_move(50.0, 30.0, nelement::EInputButton::Right);
    EXPECT_NE(cam.get_view_matrix(), before);
}

TEST(Camera_MouseMove, NoButtonHeldDoesNotChangeViewMatrix)
{
    nelement::Camera cam;
    glm::mat4 before = cam.get_view_matrix();
    cam.on_mouse_move(100.0, 100.0, nelement::EInputButton::None);
    // Only updates mCurrentPos2d, no matrix change
    EXPECT_EQ(cam.get_view_matrix(), before);
}

// ── update_self ───────────────────────────────────────────────────────────────

TEST(Camera_UpdateSelf, ChangesProjectionMatrix)
{
    nelement::Camera cam;
    glm::mat4 before = cam.get_projection_matrix();
    cam.update_self({0, 0, 5}, 60.0f, 16.0f / 9.0f, 0.1f, 200.0f);
    EXPECT_NE(cam.get_projection_matrix(), before);
}

TEST(Camera_UpdateSelf, ViewMatrixIsRecalculatedFromFocusNotInputPosition)
{
    // update_view_matrix recalculates position from focus+distance+orientation,
    // so the view matrix is stable when those don't change.
    nelement::Camera cam;
    glm::mat4 before = cam.get_view_matrix();
    cam.update_self({1, 2, 3}, 45.0f, 1.0f, 0.1f, 100.0f);
    EXPECT_EQ(cam.get_view_matrix(), before);
}
