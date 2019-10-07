#include "pch.h"
#include "IScene.h"

namespace DemoParticles
{
    
    IScene::IScene(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {
       
    }

    IScene::~IScene()
    {
        
    }
}