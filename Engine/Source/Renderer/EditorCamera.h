#pragma once

#include <glm/glm.hpp>

namespace RockEngine
{
    class EditorCamera
    {
    public:
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void OnUpdate(float deltaTime);
        void SetViewportSize(float width, float height);

        const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }

    private:
        void RecalculateView();
        void RecalculateProjection();
        void RecalculateCameraVectors();

    private:
        float m_FOV = 45.0f;
        float m_AspectRatio = 16.0f / 9.0f;
        float m_NearClip = 0.1f;
        float m_FarClip = 100.0f;

        glm::vec3 m_Position = {0.0f, 0.0f, 5.0f};

        float m_Yaw = -90.0f;
        float m_Pitch = 0.0f;

        glm::vec3 m_Forward = {0.0f, 0.0f, -1.0f};
        glm::vec3 m_Right = {1.0f, 0.0f, 0.0f};
        glm::vec3 m_Up = {0.0f, 1.0f, 0.0f};

        float m_MoveSpeed = 5.0f;
        float m_MouseSensitivity = 0.12f;

        bool m_FirstMouse = true;
        double m_LastMouseX = 0.0;
        double m_LastMouseY = 0.0;

        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
    };
}