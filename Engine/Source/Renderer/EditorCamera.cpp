#include "Renderer/EditorCamera.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace RockEngine
{
    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
    {
        RecalculateCameraVectors();
        RecalculateView();
        RecalculateProjection();
    }

    void EditorCamera::OnUpdate(float deltaTime)
    {
        GLFWwindow *window = glfwGetCurrentContext();

        float velocity = m_MoveSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_Position += m_Forward * velocity;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_Position -= m_Forward * velocity;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_Position -= m_Right * velocity;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_Position += m_Right * velocity;

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            m_Position -= m_Up * velocity;

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            m_Position += m_Up * velocity;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            double mouseX;
            double mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            if (m_FirstMouse)
            {
                m_LastMouseX = mouseX;
                m_LastMouseY = mouseY;
                m_FirstMouse = false;
            }

            float xOffset = static_cast<float>(mouseX - m_LastMouseX);
            float yOffset = static_cast<float>(m_LastMouseY - mouseY);

            m_LastMouseX = mouseX;
            m_LastMouseY = mouseY;

            xOffset *= m_MouseSensitivity;
            yOffset *= m_MouseSensitivity;

            m_Yaw += xOffset;
            m_Pitch += yOffset;

            if (m_Pitch > 89.0f)
                m_Pitch = 89.0f;

            if (m_Pitch < -89.0f)
                m_Pitch = -89.0f;

            RecalculateCameraVectors();
        }
        else
        {
            m_FirstMouse = true;
        }

        RecalculateView();
    }

    void EditorCamera::SetViewportSize(float width, float height)
    {
        if (width <= 0.0f || height <= 0.0f)
        {
            return;
        }

        m_AspectRatio = width / height;
        RecalculateProjection();
    }

    void EditorCamera::RecalculateView()
    {
        m_ViewMatrix = glm::lookAt(
            m_Position,
            m_Position + m_Forward,
            m_Up);
    }

    void EditorCamera::RecalculateProjection()
    {
        m_ProjectionMatrix = glm::perspective(
            glm::radians(m_FOV),
            m_AspectRatio,
            m_NearClip,
            m_FarClip);
    }

    void EditorCamera::RecalculateCameraVectors()
    {
        glm::vec3 forward;

        forward.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        forward.y = sin(glm::radians(m_Pitch));
        forward.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

        m_Forward = glm::normalize(forward);

        glm::vec3 worldUp = {0.0f, 1.0f, 0.0f};

        m_Right = glm::normalize(glm::cross(m_Forward, worldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
    }
}