#include "app/pch.h"

#include "elements/element.hh"
#include "elements/input.hh"

namespace nelement
{
    class Camera: public Element
    {
        private:
            glm::mat4 mViewMatrix;
		    glm::mat4 mProjection = glm::mat4{ 1.0f };
            glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
            glm::vec3 mFocus = { 0.0f, 0.0f, 0.0f };

            float mDistance = 5.0f;

            float mFov;
            float mAspect;
            float mNear;
            float mFar;

            float mPitch = 0.0f;
		    float mYaw = 0.0f;

            glm::vec2 mCurrentPos2d = { 0.0f, 0.0f };

            // constants
            const glm::vec3 cRight = { 1.0f, 0.0f, 0.0f };
            const glm::vec3 cUp = { 0.0f, 1.0f, 0.0f };
            const glm::vec3 cForward = { 0.0f, 0.0f, -1.0f };

            const float cRotationSpeed = 2.0f;

        public:
            Camera():
                mFov(45.0f),
                mNear(0.1f),
                mFar(100.0f),
                mPitch(0.0f),
                mYaw(0.0f)
            {
                update_view_matrix();
                update_projection_matrix();
            }

            void update(const glm::vec3& position, float fov, float aspect, float near, float far)
            {
                mPosition = position;
                mFov = fov;
                mAspect = aspect;
                mNear = near;
                mFar = far;

                update_view_matrix();
                update_projection_matrix();
            }

            void on_mouse_wheel(double delta)
            {
                mDistance -= static_cast<float>(delta) * cRotationSpeed;
                update_view_matrix();
            }

            void on_mouse_move(double x, double y, EInputButton button)
            {
                glm::vec2 pos2d{ x, y };

                if (button == EInputButton::Right)
                {
                    glm::vec2 delta = (pos2d - mCurrentPos2d) * 0.004f;

                    float sign = get_up().y < 0 ? -1.0f : 1.0f;
                    mYaw += sign * delta.x * cRotationSpeed;
                    mPitch += delta.y * cRotationSpeed;

                    update_view_matrix();
                }	
                else if (button == EInputButton::Middle)
                {
                    // TODO: Adjust pan speed for distance
                    glm::vec2 delta = (pos2d - mCurrentPos2d) * 0.003f;

                    mFocus += -get_right() * delta.x * mDistance;
                    mFocus += get_up() * delta.y * mDistance;

                    update_view_matrix();
                }

                mCurrentPos2d = pos2d;
            }

            void update_view_matrix()
            {
                mPosition =  mFocus - get_forward() * mDistance;

                glm::quat orientation = get_direction();
                mViewMatrix = glm::translate(glm::mat4(1.0f), mPosition) * glm::mat4_cast(orientation);
                mViewMatrix = glm::inverse(mViewMatrix);
            }

            void update_projection_matrix()
            {
                mProjection = glm::perspective(glm::radians(mFov), mAspect, mNear, mFar);
            }

            void reset()
            {

            }

            // Getter - Setter
            glm::quat get_direction() const
            {
                return glm::quat(glm::vec3(-mPitch, -mYaw, 0.0f));
            }

            glm::vec3 get_forward() const
            {
                return get_direction() * cForward;
            }

            glm::vec3 get_up() const
            {
                return get_direction() * cUp;
            }

            glm::vec3 get_right() const
            {
                return get_direction() * cRight;
            }

            glm::vec3* get_position_ptr()
            {
                return &mPosition;
            }
    };
} // namespace nelement
