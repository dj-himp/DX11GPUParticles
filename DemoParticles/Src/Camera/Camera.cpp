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
        //put '-' because in right handed POSITIVE rotation are COUNTERCLOCKWISE
        //so it feal more natural to me
        m_yaw -= yaw;
        m_pitch -= pitch;
        updateView();
    }

    std::vector<Vector3> Camera::getFrustrumCorners()
    {
        //half height of viewport at near plane
        float hhNear = tanf(m_fov / 2.0f) * m_nearPlane;
        //half width of viewport at near plane
        float hwNear = hhNear * m_aspectRatio;

        //half height of viewport at near plane
        float hhFar = tanf(m_fov / 2.0f) * m_farPlane;
        //half width of viewport at near plane
        float hwFar = hhFar * m_aspectRatio;

        Vector3 centerNear = m_position + m_forward * m_nearPlane;
        Vector3 centerFar = m_position + m_forward * m_farPlane;

        
        std::vector<Vector3> corners;

        Vector3 nearTopLeft     = centerNear + (m_up * hhNear) - (m_right * hwNear);
        corners.push_back(nearTopLeft);
        Vector3 nearTopRight    = centerNear + (m_up * hhNear) + (m_right * hwNear);
        corners.push_back(nearTopRight);
        Vector3 nearBottomLeft  = centerNear - (m_up * hhNear) - (m_right * hwNear);
        corners.push_back(nearBottomLeft);
        Vector3 nearBottomRight = centerNear + (m_up * hhNear) + (m_right * hwNear);
        corners.push_back(nearBottomRight);

        Vector3 farTopLeft      = centerFar + (m_up * hhFar) - (m_right * hwFar);
        corners.push_back(farTopLeft);
        Vector3 farTopRight     = centerFar + (m_up * hhFar) + (m_right * hwFar);
        corners.push_back(farTopRight);
        Vector3 farBottomLeft   = centerFar - (m_up * hhFar) - (m_right * hwFar);
        corners.push_back(farBottomLeft);
        Vector3 farBottomRight  = centerFar + (m_up * hhFar) + (m_right * hwFar);
        corners.push_back(farBottomRight);

        for (int i = 0; i < corners.size(); ++i)
        {
            corners[i] = Vector3::Transform(corners[i], m_view);
        }
        
        return corners;
    }

    void Camera::updateView()
    {
        Quaternion pitch = Quaternion::CreateFromAxisAngle(Vector3::UnitX, m_pitch);
        Quaternion yaw = Quaternion::CreateFromAxisAngle(Vector3::UnitY, m_yaw);

        Quaternion orientation = pitch * yaw;
        orientation.Normalize();
        Matrix rotate = Matrix::CreateFromQuaternion(orientation);
        Matrix translate = Matrix::CreateTranslation(m_position);

        m_world = rotate * translate;

        m_forward = m_world.Forward();
        m_right = m_world.Right();
        m_up = m_world.Up();

        m_world.Invert(m_view);
    }

    void Camera::updateProjection()
    {
        m_projection = Matrix::CreatePerspectiveFieldOfView(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
    }
}