#include "pch.h"
#include "ModelMesh.h"

namespace DemoParticles
{
    ModelMesh::ModelMesh(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {

    }

    //ModelMesh::~ModelMesh() = default;

    void ModelMesh::AddTextureDiffuse(const std::string& /*path*/)
    {
        //m_diffuseTexture = TextureLoader.CreateTexture2DFromBitmap(device, TextureLoader.LoadBitmap(new SharpDX.WIC.ImagingFactory2(), path));
        //m_diffuseTextureView = new ShaderResourceView(device, m_diffuseTexture);
    }

}
