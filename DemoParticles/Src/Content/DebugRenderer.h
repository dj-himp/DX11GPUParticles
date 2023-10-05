#pragma once
#include "IRenderable.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class Model;
    class VertexShader;
    class PixelShader;

    class DebugRenderer
    {
    public:

        static DebugRenderer& instance();

        void setDeviceResources(const DX::DeviceResources* deviceResources) { m_deviceResources = deviceResources; }

        void release();

        void createDeviceDependentResources();
        void createWindowSizeDependentResources();

        void releaseDeviceDependentResources();

        void update(DX::StepTimer const& timer, Camera* camera = nullptr);
       
        void render();

        void pushBackModel(std::unique_ptr<Model> model, glm::mat4 world = glm::mat4());

    private:

        const DX::DeviceResources* m_deviceResources;

        DebugRenderer();

        struct DebugModel
        {
            std::unique_ptr<Model> m_model;
            glm::mat4 m_world;

            DebugModel(std::unique_ptr<Model> model, glm::mat4 world)
            {
                m_model = std::move(model);
                m_world = world;
            }
        };

        void renderDebugModel(DebugModel& debugModel);

        std::vector<DebugModel> m_models;

        WorldConstantBuffer   m_constantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
        std::unique_ptr<VertexShader> m_debugVS;
        std::unique_ptr<PixelShader> m_debugPS;
    };
}
