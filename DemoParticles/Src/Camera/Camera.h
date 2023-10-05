#pragma once

namespace DemoParticles
{
    class Camera
    {
    public:
        Camera(glm::vec3 position, float yaw, float pitch, float roll, float aspectRatio, float fov = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);

        void setPosition(glm::vec3 position);
        void movePositionAbsolute(glm::vec3 movement);
        void movePositionRelative(glm::vec3 movement);
        glm::vec3 getPosition() { return m_position; }

        void setForward(glm::vec3 forward);
        glm::vec3 getForward() { return m_forward; }

        glm::vec3 getRight() { return m_right; }
        glm::vec3 getUp() { return m_up; }

        void setFov(float fov);
        float getFov() { return m_fov; }

        void setAspectRatio(float aspectRatio);
        float getAspectRatio() { return m_aspectRatio; }

        void setNearFarPlane(float nearPlane, float farPlane);

        void rotate(float yaw, float pitch);

        glm::mat4 getView() { return m_view; }
        glm::mat4 getProjection() { return m_projection; }
        glm::mat4 getViewProjection() { return m_view * m_projection; }

        std::vector<glm::vec3> getFrustrumCorners();

    private:

        void updateView();
        void updateProjection();

        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

        float m_yaw = 0.0f;
        float m_pitch = 0.0f;
        float m_roll = 0.0f;

        //in Radian
        float m_fov = DirectX::XM_PI / 4.0f;
        float m_aspectRatio = 16.0f/9.0f;
        float m_nearPlane = 0.1f;
        float m_farPlane = 1000.0f;

        glm::mat4 m_world; //camera world
        glm::mat4 m_view; //inverse of camera world
        glm::mat4 m_projection;
    };
}
