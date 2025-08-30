#pragma once

#include "app/pch.h"

namespace nelement
{
    enum class EInputButton
    {
        Left    = 0,
        Right   = 1,
        Middle  = 2,
        None    = 3
    };

    class Input
    {
        public:
            static EInputButton get_mouse_button(GLFWwindow* window)
            {
                EInputButton result = EInputButton::None;
                
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                    result = EInputButton::Left;
                else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
                    result = EInputButton::Right;
                else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
                    result = EInputButton::Middle;

                return result;
            }   
    };
} // namespace nelement
