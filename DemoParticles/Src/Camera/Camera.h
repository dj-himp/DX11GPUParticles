#pragma once

namespace DemoParticles
{
    class Camera
    {
    public:
        Camera(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 forward, float aspectRatio, float fov = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);
        Camera(DirectX::SimpleMath::Vector3 position, float yaw, float pitch, float roll, float aspectRatio, float fov = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);

        void setPosition(DirectX::SimpleMath::Vector3 position);
        void movePositionAbsolute(DirectX::SimpleMath::Vector3 movement);
        void movePositionRelative(DirectX::SimpleMath::Vector3 movement);
        DirectX::SimpleMath::Vector3 getPosition() { return m_position; }

        void setForward(DirectX::SimpleMath::Vector3 forward);
        DirectX::SimpleMath::Vector3 getForward() { return m_forward; }

        DirectX::SimpleMath::Vector3 getRight() { return m_right; }
        DirectX::SimpleMath::Vector3 getUp() { return m_up; }

        void setFov(float fov);
        float getFov() { return m_fov; }

        void setaspectRatio(float aspectRatio);
        float getaspectRatio() { return m_aspectRatio; }

        void setNearFarPlane(float nearPlane, float farPlane);

        void rotate(float yaw, float pitch);

        DirectX::SimpleMath::Matrix getView() { return m_view; }
        DirectX::SimpleMath::Matrix getProjection() { return m_projection; }
        DirectX::SimpleMath::Matrix getViewProjection() { return m_view * m_projection; }

        std::vector<DirectX::SimpleMath::Vector3> getFrustrumCorners();

    private:

        void updateView();
        void updateProjection();

        DirectX::SimpleMath::Vector3 m_position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
        DirectX::SimpleMath::Vector3 m_forward = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
        DirectX::SimpleMath::Vector3 m_right = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
        DirectX::SimpleMath::Vector3 m_up = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

        float m_yaw = 0.0f;
        float m_pitch = 0.0f;
        float m_roll = 0.0f;

        float m_fov = 60.0f;
        float m_aspectRatio = 16.0f/9.0f;
        float m_nearPlane = 0.1f;
        float m_farPlane = 1000.0f;

        DirectX::SimpleMath::Matrix m_world; //camera world
        DirectX::SimpleMath::Matrix m_view; //inverse of camera world
        DirectX::SimpleMath::Matrix m_projection;
    };
}
