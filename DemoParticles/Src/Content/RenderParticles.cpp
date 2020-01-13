#include "pch.h"
#include "RenderParticles.h"

#include "Common/VertexShader.h"
#include "Common/GeometryShader.h"
#include "Common/PixelShader.h"
#include "Common/ComputeShader.h"
#include "Camera/Camera.h"
#include "Common/SortLib.h"
#include "Content/DebugRenderer.h"
#include "Model/MeshFactory.h"
#include "Model/Model.h"
#include "ParticleEmitterSphere.h"
#include "ParticleEmitterPoint.h"
#include "ParticleEmitterCube.h"
#include "ParticleEmitterBuffer.h"

#include <filesystem>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderParticles::RenderParticles(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
        m_simulateParticlesBufferData.aizamaParams1 = Vector4(0.41f, 0.45f, 0.27f, 6.5f);
        m_simulateParticlesBufferData.aizamaParams2 = Vector4(0.75f, 3.0f, 0.0f, 0.0f);
        m_simulateParticlesBufferData.lorenzParams1 = Vector4(10.0f, 8.0f/3.0f, 10.0f, 0.0f);
        m_simulateParticlesBufferData.dragCoefficient = 0.001f;
        m_simulateParticlesBufferData.curlCoefficient = 1.0f;
        m_simulateParticlesBufferData.forceFieldForceScale = 1.0f;
        m_simulateParticlesBufferData.forceFieldIntensity = 1.0f;

        for (const auto& file : std::filesystem::directory_iterator("."))
        {
            if (file.path().extension().compare(".fga") == 0 || file.path().extension().compare(".vf") == 0)
            {               
                m_forceFieldList.emplace_back(file.path().filename().string());
            }
        }
    }

    void RenderParticles::createDeviceDependentResources()
    {
        
        //MAIN PARTICLE POOL

        D3D11_BUFFER_DESC particleBufferDesc;
        particleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        particleBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        particleBufferDesc.CPUAccessFlags = 0;
        particleBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        particleBufferDesc.ByteWidth = sizeof(Particle) * m_maxParticles;
        particleBufferDesc.StructureByteStride = sizeof(Particle);

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&particleBufferDesc, nullptr, &m_particleBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC particleUAVDesc;
        particleUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        particleUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        particleUAVDesc.Buffer.FirstElement = 0;
        particleUAVDesc.Buffer.NumElements = m_maxParticles;
        particleUAVDesc.Buffer.Flags = 0;
        
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_particleBuffer.Get(), &particleUAVDesc, &m_particleUAV)
        );

        D3D11_SHADER_RESOURCE_VIEW_DESC particleSRVDesc;
        particleSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        particleSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        particleSRVDesc.Buffer.FirstElement = 0;
        particleSRVDesc.Buffer.NumElements = m_maxParticles;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateShaderResourceView(m_particleBuffer.Get(), &particleSRVDesc, &m_particleSRV)
        );

        //DEAD LIST
        D3D11_BUFFER_DESC deadListBufferDesc;
        deadListBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        deadListBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        deadListBufferDesc.CPUAccessFlags = 0;
        deadListBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        deadListBufferDesc.ByteWidth = sizeof(UINT) * m_maxParticles;
        deadListBufferDesc.StructureByteStride = sizeof(UINT);

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&deadListBufferDesc, nullptr, &m_deadListBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC deadListUAVDesc;
        deadListUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        deadListUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        deadListUAVDesc.Buffer.FirstElement = 0;
        deadListUAVDesc.Buffer.NumElements = m_maxParticles;
        deadListUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_deadListBuffer.Get(), &deadListUAVDesc, &m_deadListUAV)
        );


        //ALIVE PARTICLE LIST
        D3D11_BUFFER_DESC aliveIndexBufferDesc;
        aliveIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        aliveIndexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        aliveIndexBufferDesc.CPUAccessFlags = 0;
        aliveIndexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        aliveIndexBufferDesc.ByteWidth = sizeof(ParticleIndexElement) * m_maxParticles;
        aliveIndexBufferDesc.StructureByteStride = sizeof(ParticleIndexElement);

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&aliveIndexBufferDesc, nullptr, &m_aliveIndexBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC aliveIndexUAVDesc;
        aliveIndexUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        aliveIndexUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        aliveIndexUAVDesc.Buffer.FirstElement = 0;
        aliveIndexUAVDesc.Buffer.NumElements = m_maxParticles;
        aliveIndexUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_aliveIndexBuffer.Get(), &aliveIndexUAVDesc, &m_aliveIndexUAV)
        );

        D3D11_SHADER_RESOURCE_VIEW_DESC aliveIndexSRVDesc;
        aliveIndexSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        aliveIndexSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        aliveIndexSRVDesc.Buffer.FirstElement = 0;
        aliveIndexSRVDesc.Buffer.NumElements = m_maxParticles;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateShaderResourceView(m_aliveIndexBuffer.Get(), &aliveIndexSRVDesc, &m_aliveIndexSRV)
        );

        m_initDeadListShader = std::make_unique<ComputeShader>(m_deviceResources);
        m_initDeadListShader->load(L"ResetParticles_CS.cso");

        //DeadList CONSTANT BUFFER
        CD3D11_BUFFER_DESC deadListCountConstantBufferDesc(sizeof(DeadListCountConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_deadListCountConstantBuffer)
        );
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_aliveListCountConstantBuffer)
        );

        //Indirect Draw Args Buffer
        D3D11_BUFFER_DESC indirectDrawArgsBuffer;
        ZeroMemory(&indirectDrawArgsBuffer, sizeof(indirectDrawArgsBuffer));
        indirectDrawArgsBuffer.Usage = D3D11_USAGE_DEFAULT;
        indirectDrawArgsBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        indirectDrawArgsBuffer.ByteWidth = 5 * sizeof(UINT);
        indirectDrawArgsBuffer.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&indirectDrawArgsBuffer, nullptr, &m_indirectDrawArgsBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC indirectDrawArgsUAVDesc;
        indirectDrawArgsUAVDesc.Format = DXGI_FORMAT_R32_UINT;
        indirectDrawArgsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        indirectDrawArgsUAVDesc.Buffer.FirstElement = 0;
        indirectDrawArgsUAVDesc.Buffer.NumElements = 5;
        indirectDrawArgsUAVDesc.Buffer.Flags = 0;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_indirectDrawArgsBuffer.Get(), &indirectDrawArgsUAVDesc, &m_indirectDrawArgsUAV)
        );

        CD3D11_BUFFER_DESC particlesGlobalSettingsDesc(sizeof(m_particlesGlobalSettingsBufferData), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&particlesGlobalSettingsDesc, nullptr, &m_particlesGlobalSettingsBuffer)
        );

        CD3D11_BUFFER_DESC simulateParticlesDesc(sizeof(SimulateParticlesConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&simulateParticlesDesc, nullptr, &m_simulateParticlesBuffer)
        );

        D3D11_BUFFER_DESC attractorsDesc;
        attractorsDesc.Usage = D3D11_USAGE_DEFAULT;
        attractorsDesc.ByteWidth = sizeof(Attractor) * MAX_ATTRACTORS;
        attractorsDesc.StructureByteStride = sizeof(Attractor);
        attractorsDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        attractorsDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        attractorsDesc.CPUAccessFlags = 0;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&attractorsDesc, nullptr, &m_attractorsBuffer)
        );

        D3D11_SHADER_RESOURCE_VIEW_DESC attractorsSRVDesc;
        attractorsSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        attractorsSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        attractorsSRVDesc.Buffer.FirstElement = 0;
        attractorsSRVDesc.Buffer.NumElements = MAX_ATTRACTORS;
        
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateShaderResourceView(m_attractorsBuffer.Get(), &attractorsSRVDesc, &m_attractorsSRV)
        );

        DX::ThrowIfFailed(
            CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"noisy-texture-256x256.dds", &m_noiseTexture, &m_noiseTextureSRV)
        );

        m_simulateShader = std::make_unique<ComputeShader>(m_deviceResources);
        m_simulateShader->load(L"SimulateParticles_CS.cso");

        m_sortLib = std::make_unique<SortLib>();
        m_sortLib->init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());

        initAttractors();
        initEmitters();
        initForceField();
    }

    void RenderParticles::createWindowSizeDependentResources()
    {
        
    }

    void RenderParticles::releaseDeviceDependentResources()
    {
        
    }

    void RenderParticles::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        if (!camera)
            assert(0);

        float scale = 0.01f;
        Vector3 volumeOffset = m_forceFieldList[m_currentlyLoadedForceField].m_content.boundMin;
        Vector3 volumeScale = m_forceFieldList[m_currentlyLoadedForceField].m_content.boundMax - m_forceFieldList[m_currentlyLoadedForceField].m_content.boundMin;
        Matrix volume2World = Matrix::CreateScale(volumeScale * scale) * Matrix::CreateTranslation(volumeOffset * scale);
        //m_simulateParticlesBufferData.forceFieldVolume2World = volume2World;
        m_simulateParticlesBufferData.forceFieldWorld2Volume = volume2World.Invert();
        m_simulateParticlesBufferData.forceFieldWorld2Volume = m_simulateParticlesBufferData.forceFieldWorld2Volume.Transpose();

        for (auto&& emitter : m_particleEmitters)
        {
            emitter->update(timer);
        }

        if (m_renderForceField)
        {
            updateForceField();
        }
    }

    void RenderParticles::render()
    {
        m_deviceResources->PIXBeginEvent(L"Particles");

        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_particlesGlobalSettingsBuffer.Get(), 0, nullptr, &m_particlesGlobalSettingsBufferData, 0, 0);
        //context->UpdateSubresource(m_simulateParticlesBuffer.Get(), 0, nullptr, &m_particlesGlobalSettingsBufferData, 0, 0);

        
        context->PSSetConstantBuffers(1, 1, m_particlesGlobalSettingsBuffer.GetAddressOf());
        context->VSSetConstantBuffers(1, 1, m_particlesGlobalSettingsBuffer.GetAddressOf());
        context->GSSetConstantBuffers(1, 1, m_particlesGlobalSettingsBuffer.GetAddressOf());
        context->CSSetConstantBuffers(1, 1, m_particlesGlobalSettingsBuffer.GetAddressOf());

        if (m_resetParticles)
        {
            m_deviceResources->PIXSetMarker(L"Reset");
            resetParticles();
            
            m_resetParticles = false;
        }

        m_deviceResources->PIXSetMarker(L"Emit");
        emitParticles();
        GpuProfiler::instance().setTimestamp(GpuProfiler::TS_Emit);

        m_deviceResources->PIXSetMarker(L"Simulate");
        simulateParticles();
        GpuProfiler::instance().setTimestamp(GpuProfiler::TS_Simulate);

        if (m_sortParticles)
        {
            m_deviceResources->PIXSetMarker(L"Sort");
            m_sortLib->run(m_maxParticles, m_aliveIndexUAV.Get(), m_aliveListCountConstantBuffer.Get());
        }
        GpuProfiler::instance().setTimestamp(GpuProfiler::TS_Sort);

        m_deviceResources->PIXSetMarker(L"Render");
        context->VSSetShader(m_renderParticleVS->getVertexShader(), nullptr, 0);
        context->GSSetShader(m_renderParticleGS->getGeometryShader(), nullptr, 0);
        context->PSSetShader(m_renderParticlePS->getPixelShader(), nullptr, 0);

        ID3D11Buffer* nullVertexBuffer = nullptr;
        UINT stride = 0;
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &nullVertexBuffer, &stride, &offset);
        context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

        ID3D11ShaderResourceView* vertexShaderSRVs[] = { m_particleSRV.Get(), m_aliveIndexSRV.Get() };
        context->VSSetShaderResources(0, ARRAYSIZE(vertexShaderSRVs), vertexShaderSRVs);
        context->VSSetConstantBuffers(3, 1, m_aliveListCountConstantBuffer.GetAddressOf());

        const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        switch (ParticlesGlobals::g_blendMode)
        {
        case 0:
            context->OMSetBlendState(RenderStatesHelper::Opaque().Get(), blendFactor, 0xffffffff);
            context->OMSetDepthStencilState(RenderStatesHelper::DepthDefault().Get(), 0);
            break;
        case 1:
            context->OMSetBlendState(RenderStatesHelper::NonPremultiplied().Get(), blendFactor, 0xffffffff);
            context->OMSetDepthStencilState(RenderStatesHelper::DepthRead().Get(), 0);
            break;
        case 2:
            context->OMSetBlendState(RenderStatesHelper::Additive().Get(), blendFactor, 0xffffffff);
            context->OMSetDepthStencilState(RenderStatesHelper::DepthNone().Get(), 0);
            break;
        default:
            break;
        }
        //context->OMSetBlendState(RenderStatesHelper::NonPremultiplied().Get(), blendFactor, 0xffffffff);
        //context->OMSetDepthStencilState(RenderStatesHelper::DepthRead().Get(), 0);
        if(ParticlesGlobals::g_cullNone)
            context->RSSetState(RenderStatesHelper::CullNone().Get());
        else
            context->RSSetState(RenderStatesHelper::CullCounterClockwise().Get());

        context->DrawInstancedIndirect(m_indirectDrawArgsBuffer.Get(), 0);

        ZeroMemory(vertexShaderSRVs, sizeof(vertexShaderSRVs));
        context->VSSetShaderResources(0, ARRAYSIZE(vertexShaderSRVs), vertexShaderSRVs);

        context->GSSetShader(nullptr, nullptr, 0);

        if (m_renderForceField)
        {
            renderForceField();
        }
        GpuProfiler::instance().setTimestamp(GpuProfiler::TS_Render);

        m_deviceResources->PIXEndEvent();
    }

    void RenderParticles::RenderImGui(Camera* camera)
    {
        if (ImGui::CollapsingHeader("Emitters"))
        {
            for (auto&& emitter : m_particleEmitters)
            {
                emitter->RenderImGui(camera);
            }
        }

        if (ImGui::CollapsingHeader("Simulation"))
        {
            if (ImGui::Button("Reset"))
            {
                resetParticles();
            }

            ImGui::Checkbox("Sorted", &m_sortParticles);

            if (ImGui::TreeNode("ForceField"))
            {
                ImGui::Checkbox("Enabled", (bool*)&m_simulateParticlesBufferData.addForceField);
                
                if (ImGui::BeginCombo("file", m_currentForceField.c_str()))
                {
                    for (int i = 0; i < m_forceFieldList.size(); ++i)
                    {
                        bool isSelected = (m_currentForceField == m_forceFieldList[i].m_fileName);
                        if (ImGui::Selectable(m_forceFieldList[i].m_fileName.c_str(), isSelected))
                        {
                            m_currentForceField = m_forceFieldList[i].m_fileName;
                            //m_currentlyLoadedForceField = i;
                            initForceField();
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                
                const char* modes[] = { "Wrap", "Border", "Clamp" };
                ImGui::Combo("Sample Mode", &m_forceFieldSampleMode, modes, 3);

                ImGui::DragFloat("Force scale", &m_simulateParticlesBufferData.forceFieldForceScale);
                ImGui::DragFloat("Intensity", &m_simulateParticlesBufferData.forceFieldIntensity, 0.05f, 0.0f, 1.0f);
                ImGui::Checkbox("Render", &m_renderForceField);

                

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Aizama attractor"))
            {
                ImGui::Checkbox("Enabled", (bool*)&m_simulateParticlesBufferData.addAizama);

                ImGui::DragFloat4("abcd", (float*)&m_simulateParticlesBufferData.aizamaParams1, 0.1f);
                ImGui::DragFloat2("ef", (float*)&m_simulateParticlesBufferData.aizamaParams2, 0.1f);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Lorenz attractor"))
            {
                ImGui::Checkbox("Enabled", (bool*)&m_simulateParticlesBufferData.addLorenz);

                ImGui::DragFloat3("abc", (float*)&m_simulateParticlesBufferData.lorenzParams1, 0.1f);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Curl noise"))
            {
                ImGui::Checkbox("Enabled", (bool*)&m_simulateParticlesBufferData.addCurlNoise);
                ImGui::DragFloat("Coefficient", &m_simulateParticlesBufferData.curlCoefficient, 0.1f);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Drag"))
            {
                ImGui::Checkbox("Enabled", (bool*)&m_simulateParticlesBufferData.addDrag);
                ImGui::DragFloat("Coefficient", &m_simulateParticlesBufferData.dragCoefficient, 0.001f, 0.0f);

                ImGui::TreePop();
            }
        }
    }

    void RenderParticles::setShaderResourceViews(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> positionView, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalView)
    {
        m_positionView = positionView;
        m_normalView = normalView;
    }

    void RenderParticles::save(json& file)
    {
        for (auto&& emitter : m_particleEmitters)
        {
            emitter->save(file);
        }

        file["Simulation"]["Aizama"]["Enabled"] = m_simulateParticlesBufferData.addAizama;
        file["Simulation"]["Aizama"]["AizamaParams1"] = { m_simulateParticlesBufferData.aizamaParams1.x, m_simulateParticlesBufferData.aizamaParams1.y, m_simulateParticlesBufferData.aizamaParams1.z, m_simulateParticlesBufferData.aizamaParams1.w };
        file["Simulation"]["Aizama"]["AizamaParams2"] = { m_simulateParticlesBufferData.aizamaParams2.x, m_simulateParticlesBufferData.aizamaParams2.y, m_simulateParticlesBufferData.aizamaParams2.z, m_simulateParticlesBufferData.aizamaParams2.w };

        file["Simulation"]["Lorenz"]["Enabled"] = m_simulateParticlesBufferData.addLorenz;
        file["Simulation"]["Lorenz"]["lorenzParams1"] = { m_simulateParticlesBufferData.lorenzParams1.x, m_simulateParticlesBufferData.lorenzParams1.y, m_simulateParticlesBufferData.lorenzParams1.z, m_simulateParticlesBufferData.lorenzParams1.w };

        file["Simulation"]["Lorenz"]["Enabled"] = m_simulateParticlesBufferData.addLorenz;

        file["Simulation"]["ForceField"]["Enabled"] = m_simulateParticlesBufferData.addForceField;
        file["Simulation"]["ForceField"]["CurrentForceField"] = m_currentForceField;
        file["Simulation"]["ForceField"]["Render"] = m_renderForceField;
        file["Simulation"]["ForceField"]["ForceScale"] = m_simulateParticlesBufferData.forceFieldForceScale;
        file["Simulation"]["ForceField"]["Intensity"] = m_simulateParticlesBufferData.forceFieldIntensity;

        file["Simulation"]["CurlNoise"]["Enabled"] = m_simulateParticlesBufferData.addCurlNoise;
        file["Simulation"]["CurlNoise"]["Curl Coefficient"] = m_simulateParticlesBufferData.curlCoefficient;

        file["Simulation"]["Drag"]["Enabled"] = m_simulateParticlesBufferData.addDrag;
        file["Simulation"]["Drag"]["Drag Coefficient"] = m_simulateParticlesBufferData.dragCoefficient;
    }

    void RenderParticles::resetParticles()
    {
        for (auto&& emitter : m_particleEmitters)
        {
            emitter->reset();
        }

        UINT initialCount[] = { 0 };
        m_initDeadListShader->setUAV(0, m_deadListUAV, initialCount);
        initialCount[0] = (UINT)-1;
        m_initDeadListShader->setUAV(1, m_particleUAV, initialCount);
        m_initDeadListShader->begin();
        m_initDeadListShader->start(DX::align(m_maxParticles, 256) / 256, 1, 1);
        m_initDeadListShader->end();
        m_initDeadListShader->setUAV(0, nullptr);
        m_initDeadListShader->setUAV(1, nullptr);

        //int i = m_initDeadListShader->readCounter(m_deadListUAV);
    }

    void RenderParticles::emitParticles()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        //copy the deadList counter to a constantBuffer
        context->CopyStructureCount(m_deadListCountConstantBuffer.Get(), 0, m_deadListUAV.Get());

        //global constant buffers
        context->CSSetConstantBuffers(2, 1, m_deadListCountConstantBuffer.GetAddressOf());
        
        UINT initialCounts[] = { (UINT)-1, (UINT)-1 };
        ID3D11UnorderedAccessView* uavs[] = { m_deadListUAV.Get(), m_particleUAV.Get() };
        context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);

        for (auto&& emitter : m_particleEmitters)
        {
            emitter->emit();
        }

        //clean up globals
        ZeroMemory(uavs, sizeof(uavs));
        context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

    }

    void RenderParticles::simulateParticles()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_simulateParticlesBuffer.Get(), 0, nullptr, &m_simulateParticlesBufferData, 0, 0);
        context->UpdateSubresource(m_attractorsBuffer.Get(), 0, nullptr, &m_attractorList, 0, 0);

        context->CSSetConstantBuffers(4, 1, m_simulateParticlesBuffer.GetAddressOf());

        UINT initialCount[] = { (UINT)-1 };
        m_simulateShader->setUAV(0, m_indirectDrawArgsUAV, initialCount);
        m_simulateShader->setUAV(2, m_deadListUAV, initialCount);
        m_simulateShader->setUAV(3, m_particleUAV, initialCount);
        initialCount[0] = 0;
        m_simulateShader->setUAV(1, m_aliveIndexUAV, initialCount);
        m_simulateShader->setSRV(0, m_attractorsSRV);
        m_simulateShader->setSRV(1, m_noiseTextureSRV);
        m_simulateShader->setSRV(2, m_forceFieldTextureSRV);
        switch (m_forceFieldSampleMode)
        {
        case 0:
            context->CSSetSamplers(0, 1, RenderStatesHelper::LinearWrap().GetAddressOf());
            break;
        case 1:
            context->CSSetSamplers(0, 1, RenderStatesHelper::LinearBorder().GetAddressOf());
            break;
        case 2:
            context->CSSetSamplers(0, 1, RenderStatesHelper::LinearClamp().GetAddressOf());
            break;
        default:
            break;
        }
        
        m_simulateShader->begin();
        m_simulateShader->start(DX::align(m_maxParticles, 256) / 256, 1, 1);
        m_simulateShader->end();
        m_simulateShader->setUAV(0, nullptr);
        m_simulateShader->setUAV(1, nullptr);
        m_simulateShader->setUAV(2, nullptr);
        m_simulateShader->setUAV(3, nullptr);
        m_simulateShader->setSRV(0, nullptr);
        m_simulateShader->setSRV(1, nullptr);
        m_simulateShader->setSRV(2, nullptr);

        context->CopyStructureCount(m_aliveListCountConstantBuffer.Get(), 0, m_aliveIndexUAV.Get());

        //int i = m_simulateShader->readCounter(m_aliveIndexUAV);
   }

    void RenderParticles::initAttractors()
    {
        m_simulateParticlesBufferData.nbWantedAttractors = 0;
       
        m_attractorList[0].position = Vector4(-4.0f, 2.0f, 0.0f, 1.0f);
        m_attractorList[0].gravity = 10.0f;
        m_attractorList[0].mass = 5.0f;
        m_attractorList[0].killZoneRadius = 0.5f;

        m_attractorList[1].position = Vector4(4.0f, 2.0f, 0.0f, 1.0f);
        m_attractorList[1].gravity = 10.0f;
        m_attractorList[1].mass = 5.0f;
        m_attractorList[1].killZoneRadius = 0.5f;

        m_attractorList[2].position = Vector4(0.0f, 2.0f, 4.0f, 1.0f);
        m_attractorList[2].gravity = 10.0f;
        m_attractorList[2].mass = 5.0f;
        m_attractorList[2].killZoneRadius = 0.5f;

        m_attractorList[3].position = Vector4(0.0f, 2.0f, -4.0f, 1.0f);
        m_attractorList[3].gravity = 10.0f;
        m_attractorList[3].mass = 5.0f;
        m_attractorList[3].killZoneRadius = 0.5f;
        
        //debug render
        for (UINT i = 0; i < m_simulateParticlesBufferData.nbWantedAttractors; ++i)
        {
            Matrix world = Matrix::CreateTranslation(Vector3(m_attractorList[i].position.x, m_attractorList[i].position.y, m_attractorList[i].position.z));
            DebugRenderer::instance().pushBackModel(MeshFactory::getInstance().createAxis(), world);
        }
    }

    void RenderParticles::initEmitters()
    {
        for (auto&& emitter : m_particleEmitters)
        {
            emitter->createDeviceDependentResources();

            ParticleEmitterBuffer* bufferEmitter = dynamic_cast<ParticleEmitterBuffer*>(emitter.get());
            if (bufferEmitter != nullptr)
            {
                bufferEmitter->setBuffer(m_bakedParticlesUAV);
                bufferEmitter->setIndirectArgsBuffer(m_bakedIndirectArgsBuffer);
                continue;
            }
            
            ParticleEmitterCube* cubeEmitter = dynamic_cast<ParticleEmitterCube*>(emitter.get());
            if (cubeEmitter != nullptr)
            {
                //cubeEmitter->setCubeSize(Vector3((float)m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeX, (float)m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeY, (float)m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeZ));
            }
        }
    }

    void RenderParticles::initForceField()
    {
        std::vector<ForceField>::iterator it = std::find_if(m_forceFieldList.begin(), m_forceFieldList.end(), [&](ForceField ff)->bool { return (m_currentForceField == ff.m_fileName); });
        if (it != m_forceFieldList.end())
        //if(m_forceFieldList[m_currentlyLoadedForceField].m_loaded == false)
        {
            m_currentlyLoadedForceField = std::distance(m_forceFieldList.begin(), it);

            if (m_forceFieldList[m_currentlyLoadedForceField].m_loaded == false)
            {
                FGAParser parser;
                parser.parse(m_forceFieldList[m_currentlyLoadedForceField].m_fileName.c_str(), m_forceFieldList[m_currentlyLoadedForceField].m_content);

                m_forceFieldList[m_currentlyLoadedForceField].m_loaded = true;
            }

            D3D11_TEXTURE3D_DESC desc;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;
            desc.Width = m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeX;
            desc.Height = m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeY;
            desc.Depth = m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeZ;
            desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            desc.MipLevels = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.MiscFlags = 0;

            D3D11_SUBRESOURCE_DATA data;
            data.pSysMem = &m_forceFieldList[m_currentlyLoadedForceField].m_content.forces[0];
            data.SysMemPitch = m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeX * sizeof(Vector4);
            data.SysMemSlicePitch = m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeX * m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeY * sizeof(Vector4);

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateTexture3D(&desc, &data, &m_forceFieldTexture)
            );

            D3D11_SHADER_RESOURCE_VIEW_DESC forceFieldTextureSRVDesc;
            forceFieldTextureSRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            forceFieldTextureSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
            forceFieldTextureSRVDesc.Texture3D.MipLevels = 1;
            forceFieldTextureSRVDesc.Texture3D.MostDetailedMip = 0;

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateShaderResourceView(m_forceFieldTexture.Get(), &forceFieldTextureSRVDesc, &m_forceFieldTextureSRV)
            );
        }
        
        if (m_renderParticleVS == nullptr)
        {
            //NEED REFACTOR of Shader::Load to remove this 
            std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc = {
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };

            m_renderParticleVS = std::make_unique<VertexShader>(m_deviceResources);
            m_renderParticleVS->load(L"RenderParticles_VS.cso", inputElementDesc);
        }

        if (m_renderParticleGS == nullptr)
        {
            m_renderParticleGS = std::make_unique<GeometryShader>(m_deviceResources);
            m_renderParticleGS->load(L"RenderParticles_GS.cso");
        }

        if (m_renderParticlePS == nullptr)
        {
            m_renderParticlePS = std::make_unique<PixelShader>(m_deviceResources);
            m_renderParticlePS->load(L"RenderParticles_PS.cso");
        }

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(RenderForceFieldConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_renderForceFieldConstantBuffer)
        );

        if (m_renderForceFieldVS == nullptr)
        {
            //NEED REFACTOR of Shader::Load to remove this 
            std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc;

            m_renderForceFieldVS = std::make_unique<VertexShader>(m_deviceResources);
            m_renderForceFieldVS->load(L"RenderForceField_VS.cso", inputElementDesc);
        }

        if (m_renderForceFieldGS == nullptr)
        {
            m_renderForceFieldGS = std::make_unique<GeometryShader>(m_deviceResources);
            m_renderForceFieldGS->load(L"RenderForceField_GS.cso");
        }

        if (m_renderForceFieldPS == nullptr)
        {
            m_renderForceFieldPS = std::make_unique<PixelShader>(m_deviceResources);
            m_renderForceFieldPS->load(L"RenderForceField_PS.cso");
        }
    }

    void RenderParticles::updateForceField()
    {
        m_renderForceFieldConstantBufferData.size = Vector4((float)m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeX, (float)m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeY, (float)m_forceFieldList[m_currentlyLoadedForceField].m_content.sizeZ, 1.0f);

        float scale = 0.01f;
        Vector3 volumeOffset = m_forceFieldList[m_currentlyLoadedForceField].m_content.boundMin;
        Vector3 volumeScale = m_forceFieldList[m_currentlyLoadedForceField].m_content.boundMax - m_forceFieldList[m_currentlyLoadedForceField].m_content.boundMin;
        m_renderForceFieldConstantBufferData.forceFieldVolume2World = Matrix::CreateScale(volumeScale * scale) * Matrix::CreateTranslation(volumeOffset * scale);
        //m_constantBufferData.forceFieldVolume2World = m_constantBufferData.forceFieldVolume2World.Invert();
        m_renderForceFieldConstantBufferData.forceFieldVolume2World = m_renderForceFieldConstantBufferData.forceFieldVolume2World.Transpose();
    }

    void RenderParticles::renderForceField()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_renderForceFieldConstantBuffer.Get(), 0, nullptr, &m_renderForceFieldConstantBufferData, 0, 0);

        context->VSSetShader(m_renderForceFieldVS->getVertexShader(), nullptr, 0);
        context->GSSetShader(m_renderForceFieldGS->getGeometryShader(), nullptr, 0);
        context->PSSetShader(m_renderForceFieldPS->getPixelShader(), nullptr, 0);

        ID3D11Buffer* nullVertexBuffer = nullptr;
        UINT stride = 0;
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &nullVertexBuffer, &stride, &offset);
        context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

        context->VSSetConstantBuffers(4, 1, m_renderForceFieldConstantBuffer.GetAddressOf());
        ID3D11ShaderResourceView* SRVs[] = { m_forceFieldTextureSRV.Get() };
        context->VSSetShaderResources(0, ARRAYSIZE(SRVs), SRVs);
        context->VSSetSamplers(0, 1, RenderStatesHelper::PointClamp().GetAddressOf());

        context->GSSetConstantBuffers(4, 1, m_renderForceFieldConstantBuffer.GetAddressOf());

        const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        context->OMSetBlendState(RenderStatesHelper::Opaque().Get(), blendFactor, 0xffffffff);
        context->RSSetState(RenderStatesHelper::CullNone().Get());
        context->OMSetDepthStencilState(RenderStatesHelper::DepthNone().Get(), 0);

        context->DrawInstanced((UINT)m_forceFieldList[m_currentlyLoadedForceField].m_content.forces.size(), 1, 0, 0);

        //ZeroMemory(SRVs, ARRAYSIZE(SRVs));
        SRVs[0] = nullptr;
        context->VSSetShaderResources(0, ARRAYSIZE(SRVs), SRVs);

        context->GSSetShader(nullptr, nullptr, 0);
    }

    void RenderParticles::load(json& file)
    {
        std::unique_ptr<ParticleEmitterSphere> sphereEmitter = std::make_unique<ParticleEmitterSphere>(m_deviceResources);
        m_particleEmitters.push_back(std::move(sphereEmitter));

        std::unique_ptr<ParticleEmitterPoint> pointEmitter = std::make_unique<ParticleEmitterPoint>(m_deviceResources);
        m_particleEmitters.push_back(std::move(pointEmitter));

        std::unique_ptr<ParticleEmitterCube> cubeEmitter = std::make_unique<ParticleEmitterCube>(m_deviceResources);
        m_particleEmitters.push_back(std::move(cubeEmitter));

        std::unique_ptr<ParticleEmitterBuffer> bufferEmitter = std::make_unique<ParticleEmitterBuffer>(m_deviceResources);
        m_particleEmitters.push_back(std::move(bufferEmitter));

        for (auto&& emitter : m_particleEmitters)
        {
            emitter->load(file);
        }

        m_simulateParticlesBufferData.addAizama = file["Simulation"]["Aizama"]["Enabled"];
        std::vector<float> aizamaParams1 = file["Simulation"]["Aizama"]["AizamaParams1"];
        m_simulateParticlesBufferData.aizamaParams1 = Vector4(&aizamaParams1[0]);
        std::vector<float> aizamaParams2 = file["Simulation"]["Aizama"]["AizamaParams2"];
        m_simulateParticlesBufferData.aizamaParams2 = Vector4(&aizamaParams2[0]);

        m_simulateParticlesBufferData.addLorenz = file["Simulation"]["Lorenz"]["Enabled"];
        std::vector<float> LorenzParams1 = file["Simulation"]["Lorenz"]["lorenzParams1"];
        m_simulateParticlesBufferData.lorenzParams1 = Vector4(&LorenzParams1[0]);

        m_simulateParticlesBufferData.addLorenz = file["Simulation"]["Lorenz"]["Enabled"];

        m_simulateParticlesBufferData.addForceField = file["Simulation"]["ForceField"]["Enabled"];
        m_currentForceField = file["Simulation"]["ForceField"]["CurrentForceField"];

        m_renderForceField = file["Simulation"]["ForceField"]["Render"];
        m_simulateParticlesBufferData.forceFieldForceScale = file["Simulation"]["ForceField"]["ForceScale"];
        m_simulateParticlesBufferData.forceFieldIntensity = file["Simulation"]["ForceField"]["Intensity"];

        m_simulateParticlesBufferData.addCurlNoise = file["Simulation"]["CurlNoise"]["Enabled"];
        m_simulateParticlesBufferData.curlCoefficient = file["Simulation"]["CurlNoise"]["Curl Coefficient"];

        m_simulateParticlesBufferData.addDrag = file["Simulation"]["Drag"]["Enabled"];
        m_simulateParticlesBufferData.dragCoefficient = file["Simulation"]["Drag"]["Drag Coefficient"];
    }

}
