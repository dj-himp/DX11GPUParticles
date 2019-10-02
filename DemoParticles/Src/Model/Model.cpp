#include "pch.h"
#include "Model.h"

namespace DemoParticles
{
    Model::Model(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {

    }

    //AssimpModel::~AssimpModel() = default;

    /*void AssimpModel::AddMesh(std::unique_ptr<ModelMesh> mesh)
    {
        m_meshes.push_back(mesh);
    }*/

    std::unique_ptr<ModelMesh>& Model::AddMesh()
    {
        std::unique_ptr<ModelMesh> mesh = std::make_unique<ModelMesh>(m_deviceResources);
        m_meshes.push_back(std::move(mesh));

        return m_meshes.back();
    }

    void Model::SetAABox(DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max)
    {
        m_aaBoxMin = min;
        m_aaBoxMax = max;
        m_aaBoxCentre = 0.5f * (min + max);
    }

}
