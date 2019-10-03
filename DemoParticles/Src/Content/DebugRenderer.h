#pragma once
#include "IRenderable.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class Model;   

    class DebugRenderer : public IRenderable
    {
    public:
        DebugRenderer(const DX::DeviceResources* deviceResources);

        virtual void init() override;
        virtual void release() override;

        virtual void createDeviceDependentResources() override;
        virtual void createWindowSizeDependentResources() override;

        virtual void releaseDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
       
        virtual void render() override;

        void pushBackModel(std::unique_ptr<Model> model, DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity);

    private:
        
        struct DebugModel
        {
            std::unique_ptr<Model> m_model;
            DirectX::SimpleMath::Matrix m_world;

            DebugModel(std::unique_ptr<Model> model, DirectX::SimpleMath::Matrix world)
            {
                m_model = std::move(model);
                m_world = world;
            }
        };

        void renderDebugModel(DebugModel& debugModel);

        

        std::vector<DebugModel> m_models;

        ModelConstantBuffer   m_constantBufferData;
    };
}
