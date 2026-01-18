#include <doctest/doctest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cmath>

// Camera tests - testing matrix math and input handling
// Uses standalone TestCamera to avoid shader/OpenGL dependencies

namespace camera_tests
{
    bool matrix_valid(const glm::mat4& m)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (std::isnan(m[i][j]) || std::isinf(m[i][j]))
                    return false;
            }
        }
        return true;
    }

    class TestCamera
    {
    private:
        glm::mat4 mViewMatrix;
        glm::mat4 mProjection = glm::mat4{1.0f};
        glm::vec3 mPosition = {0.0f, 0.0f, 0.0f};
        glm::vec3 mFocus = {0.0f, 0.0f, 0.0f};

        float mDistance = 5.0f;
        float mFov = 45.0f;
        float mAspect = 1.0f;
        float mNear = 0.1f;
        float mFar = 100.0f;

        float mPitch = 0.0f;
        float mYaw = 0.0f;

        glm::vec2 mCurrentPos2d = {0.0f, 0.0f};

        const glm::vec3 cRight = {1.0f, 0.0f, 0.0f};
        const glm::vec3 cUp = {0.0f, 1.0f, 0.0f};
        const glm::vec3 cForward = {0.0f, 0.0f, -1.0f};
        const float cRotationSpeed = 2.0f;

    public:
        enum class Button { None, Right, Middle };

        TestCamera()
        {
            update_view_matrix();
            update_projection_matrix();
        }

        void update_self(const glm::vec3& position, float fov, float aspect, float near, float far)
        {
            mPosition = position;
            mFov = fov;
            mAspect = aspect;
            mNear = near;
            mFar = far;
            update_view_matrix();
            update_projection_matrix();
        }

        void set_aspect(float aspect)
        {
            mProjection = glm::perspective(mFov, aspect, mNear, mFar);
        }

        void on_mouse_wheel(double delta)
        {
            mDistance -= static_cast<float>(delta) * cRotationSpeed;
            update_view_matrix();
        }

        void on_mouse_move(double x, double y, Button button)
        {
            glm::vec2 pos2d{x, y};

            if (button == Button::Right)
            {
                glm::vec2 delta = (pos2d - mCurrentPos2d) * 0.004f;
                float sign = get_up().y < 0 ? -1.0f : 1.0f;
                mYaw += sign * delta.x * cRotationSpeed;
                mPitch += delta.y * cRotationSpeed;
                update_view_matrix();
            }
            else if (button == Button::Middle)
            {
                glm::vec2 delta = (pos2d - mCurrentPos2d) * 0.003f;
                mFocus += -get_right() * delta.x * mDistance;
                mFocus += get_up() * delta.y * mDistance;
                update_view_matrix();
            }

            mCurrentPos2d = pos2d;
        }

        void update_view_matrix()
        {
            mPosition = mFocus - get_forward() * mDistance;
            glm::quat orientation = get_direction();
            mViewMatrix = glm::translate(glm::mat4(1.0f), mPosition) * glm::mat4_cast(orientation);
            mViewMatrix = glm::inverse(mViewMatrix);
        }

        void update_projection_matrix()
        {
            mProjection = glm::perspective(glm::radians(mFov), mAspect, mNear, mFar);
        }

        glm::mat4 get_view_matrix() const { return mViewMatrix; }
        glm::mat4 get_projection_matrix() const { return mProjection; }
        glm::vec3 get_position() const { return mPosition; }

        glm::quat get_direction() const { return glm::quat(glm::vec3(-mPitch, -mYaw, 0.0f)); }
        glm::vec3 get_forward() const { return get_direction() * cForward; }
        glm::vec3 get_up() const { return get_direction() * cUp; }
        glm::vec3 get_right() const { return get_direction() * cRight; }
    };
}

using TestCamera = camera_tests::TestCamera;

TEST_CASE("Camera: default construction")
{
    TestCamera camera;

    CHECK(camera_tests::matrix_valid(camera.get_view_matrix()));
    CHECK(camera_tests::matrix_valid(camera.get_projection_matrix()));
}

TEST_CASE("Camera: update_self changes matrices")
{
    TestCamera camera;

    glm::vec3 newPos(5.0f, 3.0f, -2.0f);
    camera.update_self(newPos, 60.0f, 16.0f/9.0f, 0.1f, 1000.0f);

    CHECK(camera_tests::matrix_valid(camera.get_view_matrix()));
    CHECK(camera_tests::matrix_valid(camera.get_projection_matrix()));
}

TEST_CASE("Camera: set_aspect updates projection")
{
    TestCamera camera;

    glm::mat4 proj1 = camera.get_projection_matrix();
    camera.set_aspect(2.0f);
    glm::mat4 proj2 = camera.get_projection_matrix();

    CHECK(proj1 != proj2);
    CHECK(camera_tests::matrix_valid(proj2));
}

TEST_CASE("Camera: mouse wheel changes distance")
{
    TestCamera camera;

    glm::mat4 view1 = camera.get_view_matrix();
    camera.on_mouse_wheel(1.0);
    glm::mat4 view2 = camera.get_view_matrix();

    CHECK(view1 != view2);
    CHECK(camera_tests::matrix_valid(view2));
}

TEST_CASE("Camera: mouse wheel zoom in and out")
{
    TestCamera camera;

    glm::vec3 pos1 = camera.get_position();

    camera.on_mouse_wheel(2.0);
    glm::vec3 pos2 = camera.get_position();

    camera.on_mouse_wheel(-2.0);
    glm::vec3 pos3 = camera.get_position();

    CHECK(glm::length(pos2) < glm::length(pos1));
    CHECK(glm::length(pos3) > glm::length(pos2));
}

TEST_CASE("Camera: get_direction returns valid quaternion")
{
    TestCamera camera;

    glm::quat dir = camera.get_direction();

    float len = glm::length(dir);
    CHECK(len == doctest::Approx(1.0f).epsilon(0.01f));
}

TEST_CASE("Camera: get_forward returns unit vector")
{
    TestCamera camera;

    glm::vec3 forward = camera.get_forward();

    CHECK(glm::length(forward) == doctest::Approx(1.0f).epsilon(0.01f));
}

TEST_CASE("Camera: get_up returns unit vector")
{
    TestCamera camera;

    glm::vec3 up = camera.get_up();

    CHECK(glm::length(up) == doctest::Approx(1.0f).epsilon(0.01f));
}

TEST_CASE("Camera: get_right returns unit vector")
{
    TestCamera camera;

    glm::vec3 right = camera.get_right();

    CHECK(glm::length(right) == doctest::Approx(1.0f).epsilon(0.01f));
}

TEST_CASE("Camera: forward, up, right are orthogonal")
{
    TestCamera camera;

    glm::vec3 forward = camera.get_forward();
    glm::vec3 up = camera.get_up();
    glm::vec3 right = camera.get_right();

    CHECK(glm::dot(forward, up) == doctest::Approx(0.0f).epsilon(0.01f));
    CHECK(glm::dot(forward, right) == doctest::Approx(0.0f).epsilon(0.01f));
    CHECK(glm::dot(up, right) == doctest::Approx(0.0f).epsilon(0.01f));
}

TEST_CASE("Camera: mouse move with right button rotates")
{
    TestCamera camera;

    camera.on_mouse_move(100.0, 100.0, TestCamera::Button::None);
    glm::mat4 view1 = camera.get_view_matrix();

    camera.on_mouse_move(150.0, 120.0, TestCamera::Button::Right);
    glm::mat4 view2 = camera.get_view_matrix();

    CHECK(view1 != view2);
    CHECK(camera_tests::matrix_valid(view2));
}

TEST_CASE("Camera: mouse move with middle button pans")
{
    TestCamera camera;

    camera.on_mouse_move(100.0, 100.0, TestCamera::Button::None);
    glm::mat4 view1 = camera.get_view_matrix();

    camera.on_mouse_move(150.0, 120.0, TestCamera::Button::Middle);
    glm::mat4 view2 = camera.get_view_matrix();

    CHECK(view1 != view2);
    CHECK(camera_tests::matrix_valid(view2));
}

TEST_CASE("Camera: mouse move with no button does not change view")
{
    TestCamera camera;

    camera.on_mouse_move(100.0, 100.0, TestCamera::Button::None);
    glm::mat4 view1 = camera.get_view_matrix();

    camera.on_mouse_move(200.0, 200.0, TestCamera::Button::None);
    glm::mat4 view2 = camera.get_view_matrix();

    CHECK(view1 == view2);
}
