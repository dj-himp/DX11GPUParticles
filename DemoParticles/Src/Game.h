//
// Game.h
//

#pragma once

#include "Common/DeviceResources.h"
#include "Common/StepTimer.h"


namespace DemoParticles
{
    class IRenderable;
    class CameraControllerFPS;
    class InputManager;
    class SceneMenger;
}

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize(int& width, int& height) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    std::unique_ptr<DirectX::Mouse> m_mouse;
    std::unique_ptr<DirectX::Keyboard> m_keyboard;
    std::unique_ptr<DemoParticles::InputManager> m_inputManager;

    std::vector<std::unique_ptr<DemoParticles::IRenderable>> m_renderables;
    
    std::unique_ptr<DemoParticles::SceneMenger> m_sceneMenger;

    //std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

    std::unique_ptr<DemoParticles::CameraControllerFPS> m_cameraControllerFPS;
};