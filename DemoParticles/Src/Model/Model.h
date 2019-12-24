#pragma once
#include "ModelMesh.h"

namespace DemoParticles
{

    class Model
    {
    public:

        Model(const DX::DeviceResources* deviceResources);

        DirectX::SimpleMath::Vector3 getAaBoxMin() { return m_aaBoxMin; }
        void setAaBoxMin(DirectX::SimpleMath::Vector3 aaboxMin) { m_aaBoxMin = aaboxMin; }

        DirectX::SimpleMath::Vector3 getAaBoxMax() { return m_aaBoxMax; }
        void setAaBoxMax(DirectX::SimpleMath::Vector3 aaboxMax) { m_aaBoxMax = aaboxMax; }

        DirectX::SimpleMath::Vector3 getAaBoxCentre() { return m_aaBoxCentre; }
        void setAaBoxCentre(DirectX::SimpleMath::Vector3 aaboxCentre) { m_aaBoxCentre = aaboxCentre; }

        //std::vector<ShaderResourceView> getDiffuses();
        
        std::unique_ptr<ModelMesh>& AddMesh();
        void SetAABox(DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max);

        void setInputElements(std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements) { m_inputElements = inputElements; }
        const std::vector<D3D11_INPUT_ELEMENT_DESC> getInputElements() { return m_inputElements; }

        void setVertexStride(size_t stride) { m_vertexStride = stride; }
        size_t getVertexStride() { return m_vertexStride; }

        const size_t getMeshCount() { return m_meshes.size(); }
        const std::unique_ptr<ModelMesh>& getMesh(int index) { return m_meshes[index]; }

    private:
        const DX::DeviceResources* m_deviceResources;

        std::vector<std::unique_ptr<ModelMesh>> m_meshes;
        
        std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputElements;
        size_t m_vertexStride;

        DirectX::SimpleMath::Vector3 m_aaBoxMin;
        DirectX::SimpleMath::Vector3 m_aaBoxMax;
        DirectX::SimpleMath::Vector3 m_aaBoxCentre;
    };
}