#pragma once

#include "IRenderable.h"

namespace DemoParticles
{
    class RenderParticles : public IRenderable
    {
    public:
        RenderParticles();
        ~RenderParticles();

        virtual void init() override;
        virtual void release() override;

        virtual void createDeviceDependentResources() override;
        virtual void createWindowSizeDependentResources() override;
        virtual void releaseDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        virtual void render() override;

    private:


    };
}
