#include "pch.h"
#include "Camera.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{

    Camera::Camera(DirectX::SimpleMath::Vector3 position, float yaw, float pitch, float roll, float aspectRatio, float fov /*= 60.0f*/, float nearPlane /*= 0.1f*/, float farPlane /*= 1000.0f*/)
        : m_position(position)
        , m_yaw(yaw)
        , m_pitch(pitch)
        , m_roll(roll)
        , m_aspectRatio(aspectRatio)
        , m_fov(fov)
        , m_nearPlane(nearPlane)
        , m_farPlane(farPlane)
    {
        updateView();
        updateProjection();
    }

    Camera::Camera(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 forward, float aspectRatio, float fov /*= 60.0f*/, float nearPlane /*= 0.1f*/, float farPlane /*= 1000.0f*/)
        : m_position(position)
        , m_forward(forward)
        , m_aspectRatio(aspectRatio)
        , m_fov(fov)
        , m_nearPlane(nearPlane)
        , m_farPlane(farPlane)
    {
        updateView();
        updateProjection();
    }

    void Camera::setPosition(DirectX::SimpleMath::Vector3 position)
    {
        m_position = position;
        updateView();
    }

    void Camera::movePositionAbsolute(DirectX::SimpleMath::Vector3 movement)
    {
        m_position += movement;
        updateView();
    }

    void Camera::movePositionRelative(DirectX::SimpleMath::Vector3 movement)
    {
        m_position += m_right * movement.x;
        m_position += m_up * movement.y;
        m_position += m_forward * movement.z;
        updateView();
    }

    void Camera::setForward(DirectX::SimpleMath::Vector3 forward)
    {
        m_forward = forward;
        updateView();
    }

    void Camera::setFov(float fov)
    {
        m_fov = fov;
        updateProjection();
    }

    void Camera::setaspectRatio(float aspectRatio)
    {
        m_aspectRatio = aspectRatio;
        updateProjection();
    }

    void Camera::setNearFarPlane(float nearPlane, float farPlane)
    {
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;
        updateProjection();
    }

    void Camera::rotate(float yaw, float pitch)
    {
        m_yaw += yaw;
        m_pitch += pitch;
        updateView();
    }

    void Camera::updateView()
    {
        /*m_view = Matrix::CreateTranslation(m_position);
        m_view *= Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, 0.0f);

        m_forward = m_view.Forward();
        m_right = m_view.Right();
        m_up = m_view.Up();*/

        Quaternion pitch = Quaternion::CreateFromAxisAngle(Vector3::UnitX, m_pitch);
        Quaternion yaw = Quaternion::CreateFromAxisAngle(Vector3::UnitY, m_yaw);

        Quaternion orientation = pitch * yaw;
        orientation.Normalize();
        Matrix rotate = Matrix::CreateFromQuaternion(orientation);
        Matrix translate = Matrix::CreateTranslation(m_position);

        m_view = translate * rotate;

        m_forward = m_view.Forward();
        m_right = m_view.Right();
        m_up = m_view.Up();
    }

    void Camera::updateProjection()
    {
        m_projection = Matrix::CreatePerspectiveFieldOfView(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
    }
}