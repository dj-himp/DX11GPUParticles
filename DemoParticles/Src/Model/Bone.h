#pragma once

namespace DemoParticles
{
    class Bone
    {
    public:
        Bone();
        ~Bone();

        std::string m_name;
        DirectX::SimpleMath::Matrix m_offset;
        DirectX::SimpleMath::Matrix m_localTransform;
        DirectX::SimpleMath::Matrix m_globalTransform;
        DirectX::SimpleMath::Matrix m_originalLocalTransform;

        Bone* m_parent;
        std::vector<Bone*> m_childrens;
    };
}