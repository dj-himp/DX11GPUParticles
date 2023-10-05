#include "pch.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace DemoParticles
{

    Camera::Camera(glm::vec3 position, float yaw, float pitch, float roll, float aspectRatio, float fov /*= 60.0f*/, float nearPlane /*= 0.1f*/, float farPlane /*= 1000.0f*/)
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

    void Camera::setPosition(glm::vec3 position)
    {
        m_position = position;
        updateView();
    }

    void Camera::movePositionAbsolute(glm::vec3 movement)
    {
        m_position += movement;
        updateView();
    }

    void Camera::movePositionRelative(glm::vec3 movement)
    {
        m_position += m_right * movement.x;
        m_position += m_up * movement.y;
        m_position += m_forward * movement.z;
        updateView();
    }

    void Camera::setForward(glm::vec3 forward)
    {
        m_forward = forward;
        updateView();
    }

    void Camera::setFov(float fov)
    {
        m_fov = fov;
        updateProjection();
    }

    void Camera::setAspectRatio(float aspectRatio)
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

    std::vector<glm::vec3> Camera::getFrustrumCorners()
    {

        //half height of viewport at near plane
        float hhNear = tanf(m_fov / 2.0f) * m_nearPlane;
        //half width of viewport at near plane
        float hwNear = hhNear * m_aspectRatio;

        //half height of viewport at near plane
        float hhFar = tanf(m_fov / 2.0f) * m_farPlane;
        //half width of viewport at near plane
        float hwFar = hhFar * m_aspectRatio;

        glm::vec3 centerNear = m_position + m_forward * m_nearPlane;
        glm::vec3 centerFar = m_position + m_forward * m_farPlane;

        
        std::vector<glm::vec3> corners;

        glm::vec3 nearTopLeft     = centerNear + (m_up * hhNear) - (m_right * hwNear);
        corners.push_back(nearTopLeft);
        glm::vec3 nearTopRight    = centerNear + (m_up * hhNear) + (m_right * hwNear);
        corners.push_back(nearTopRight);
        glm::vec3 nearBottomLeft  = centerNear - (m_up * hhNear) - (m_right * hwNear);
        corners.push_back(nearBottomLeft);
        glm::vec3 nearBottomRight = centerNear - (m_up * hhNear) + (m_right * hwNear);
        corners.push_back(nearBottomRight);

        glm::vec3 farTopLeft      = centerFar + (m_up * hhFar) - (m_right * hwFar);
        corners.push_back(farTopLeft);
        glm::vec3 farTopRight     = centerFar + (m_up * hhFar) + (m_right * hwFar);
        corners.push_back(farTopRight);
        glm::vec3 farBottomLeft   = centerFar - (m_up * hhFar) - (m_right * hwFar);
        corners.push_back(farBottomLeft);
        glm::vec3 farBottomRight  = centerFar - (m_up * hhFar) + (m_right * hwFar);
        corners.push_back(farBottomRight);
        
        for (auto& corner : corners)
        {
            corner = glm::normalize(corner);
        }

        /*BoundingFrustum frustum;
        BoundingFrustum::CreateFromglm::mat4(frustum, m_projection);

        std::vector<glm::vec3> corners;
        glm::vec3 cornersTab[8];
        frustum.GetCorners(cornersTab);
        for (int i = 0; i < 8; ++i)
        {
            corners.push_back(cornersTab[i]);
        }*/

        
        return corners;
    }

    void Camera::updateView()
    {
        DirectX::SimpleMath::Quaternion p = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitX, m_pitch);
        DirectX::SimpleMath::Quaternion y = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY, m_yaw);
        DirectX::SimpleMath::Quaternion o = p * y;
        o.Normalize();
        DirectX::SimpleMath::Matrix r = DirectX::SimpleMath::Matrix::CreateFromQuaternion(o);
        DirectX::SimpleMath::Matrix t = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(m_position.x, m_position.y, m_position.z));
        DirectX::SimpleMath::Matrix world = r * t;


        glm::quat pitch = glm::angleAxis(m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yaw = glm::angleAxis(m_yaw, glm::vec3(0.0f, 1.0f, 0.0f));


        glm::quat orientation = glm::normalize(pitch * yaw);
        glm::mat4 rotate = glm::mat4_cast(orientation);
        //glm::mat4 translate = glm::mat4::CreateTranslation(m_position);
        
        m_world = glm::translate(rotate, m_position);

        //m_world.Forward();
        //m_world.Right();
        //m_world.Up();

        m_view = glm::inverse(m_world);

        m_forward = -m_view[2]; //m_world.Forward();
        m_right = m_view[0]; //m_world.Right();
        m_up = m_view[1]; //m_world.Up();

    }

    void Camera::updateProjection()
    {
        m_projection = glm::perspectiveRH_ZO(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
        DirectX::SimpleMath::Matrix m_projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
    }
}