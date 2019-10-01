#include "pch.h"
#include "RenderFullscreenQuad.h"

#include "../Common/DirectXHelper.h"
#include "../Model/Model.h"
#include "../Camera/Camera.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderFullscreenQuad::RenderFullscreenQuad(const std::shared_ptr<DX::DeviceResources>& deviceResources)
        : IRenderable(deviceResources)
    {
        init();
    }

    void RenderFullscreenQuad::init()
    {
        IRenderable::init();
    }

    void RenderFullscreenQuad::release()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void RenderFullscreenQuad::createDeviceDependentResources()
    {
        auto loadVSTask = DX::ReadDataAsync(L"RenderQuad_VS.cso");
        auto loadPSTask = DX::ReadDataAsync(L"RenderQuad_PS.cso");

        // After the vertex shader file is loaded, create the shader and input layout.
        auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
            DX::ThrowIfFailed(                
                m_deviceResources->GetD3DDevice()->CreateVertexShader(
                    &fileData[0],
                    fileData.size(),
                    nullptr,
                    &m_vertexShader
                )
            );

            m_quadCreator = std::make_unique<QuadCreator>(m_deviceResources);
            m_quad = m_quadCreator->create();

            std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDesc = m_quad->getInputElements();

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateInputLayout(
                    vertexDesc.data(),
                    vertexDesc.size(),
                    &fileData[0],
                    fileData.size(),
                    &m_inputLayout
                )
            );
        });

        

        // After the pixel shader file is loaded, create the shader and constant buffer.
        auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreatePixelShader(
                    &fileData[0],
                    fileData.size(),
                    nullptr,
                    &m_pixelShader
                )
            );

            CD3D11_BUFFER_DESC constantBufferDesc(sizeof(QuadConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateBuffer(
                    &constantBufferDesc,
                    nullptr,
                    &m_constantBuffer
                )
            );
        });

        // Once the quad is loaded, the object is ready to be rendered.
        (createPSTask && createVSTask).then([this]() {

            m_loadingComplete = true;
        });
    }

    void RenderFullscreenQuad::createWindowSizeDependentResources()
    {
        //NOTHING THERE
    }

    void RenderFullscreenQuad::releaseDeviceDependentResources()
    {
        m_loadingComplete = false;
        m_vertexShader.Reset();
        m_inputLayout.Reset();
        m_pixelShader.Reset();
        m_constantBuffer.Reset();
    }

    void RenderFullscreenQuad::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        //TEST
        m_posScale = Matrix::CreateTranslation(Vector3(0.0f, 0.0f, 0.0f)) * Matrix::CreateScale(1.0f);
        XMStoreFloat4x4(&m_constantBufferData.posScale, m_posScale.Transpose());
    }

    void RenderFullscreenQuad::render()
    {
        // Loading is asynchronous. Only draw geometry after it's loaded.
        if(!m_loadingComplete)
        {
            return;
        }

        auto context = m_deviceResources->GetD3DDeviceContext();

        // Prepare the constant buffer to send it to the graphics device.
        context->UpdateSubresource1(
            m_constantBuffer.Get(),
            0,
            NULL,
            &m_constantBufferData,
            0,
            0,
            0
        );

        // Each vertex is one instance of the VertexPositionUV struct.
        UINT stride = sizeof(VertexPositionUV);
        UINT offset = 0;
        context->IASetVertexBuffers(
            0,
            1,
            m_quad->getMesh(0)->getVertexBuffer().GetAddressOf(),
            &stride,
            &offset
        );

        context->IASetIndexBuffer(
            m_quad->getMesh(0)->getIndexBuffer().Get(),
            DXGI_FORMAT_R32_UINT,
            0
        );

        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->IASetInputLayout(m_inputLayout.Get());

        // Attach our vertex shader.
        context->VSSetShader(
            m_vertexShader.Get(),
            nullptr,
            0
        );

        // Send the constant buffer to the graphics device.
        context->VSSetConstantBuffers1(
            0,
            1,
            m_constantBuffer.GetAddressOf(),
            nullptr,
            nullptr
        );

        // Attach our pixel shader.
        context->PSSetShader(
            m_pixelShader.Get(),
            nullptr,
            0
        );

        // Draw the objects.
        context->DrawIndexed(
            m_quad->getMesh(0)->getIndexCount(),
            0,
            0
        );
    }

}