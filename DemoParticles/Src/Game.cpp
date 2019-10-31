//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "imgui/imgui.h"
#include "Gui/imgui_impl_win32.h"
#include "Gui/imgui_impl_dx11.h"

//#include "Content/SampleFpsTextRenderer.h"
#include "Camera/CameraControllerFPS.h"
#include "Common/InputManager.h"
#include "Content/RenderModel.h"
#include "Content/MengerRenderer.h"
#include "Content/RenderFullscreenQuad.h"
#include "Content/BakeModelParticles.h"

#include "Model/MeshFactory.h"
#include "Content/DebugRenderer.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Scene/SceneMenger.h"
#include "Common/RenderStatesHelper.h"

extern void ExitGame();

using namespace DirectX;
using namespace DemoParticles;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    //m_deviceResources = std::make_unique<DX::DeviceResources>();
    
    //disable VSync
    //m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT, 2, D3D_FEATURE_LEVEL_11_0, DX::DeviceResources::c_FlipPresent | DX::DeviceResources::c_AllowTearing);

    //enable gamma correction;
    m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 2, D3D_FEATURE_LEVEL_11_0, DX::DeviceResources::c_FlipPresent);

    m_deviceResources->RegisterDeviceNotify(this);

    m_inputManager = std::make_unique<InputManager>();

}


//needed to use forward declaration of a unique_ptr : 
//default destructor is inline so the unique_ptr type is incomplete in the header
Game::~Game()
{

}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_mouse = std::make_unique<DirectX::Mouse>();
    m_keyboard = std::make_unique<DirectX::Keyboard>();
    
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    //m_fpsTextRenderer = std::make_unique<SampleFpsTextRenderer>(m_deviceResources);

    //init MeshFactory
    MeshFactory::getInstance().setDeviceResources(m_deviceResources.get());

    //Camera camera(DirectX::SimpleMath::Vector3(5.0f, 0.0f, 0.0f), XM_PI / 2.0f, 0.0f, 0.0f, width/height, XM_PI / 4.0f, 0.1f, 10.0f);
    //std::vector< DirectX::SimpleMath::Vector3> corners = camera.getFrustrumCorners();


    //std::unique_ptr<DebugRenderer> debugRenderer = std::make_unique<DebugRenderer>(m_deviceResources.get());

    DebugRenderer::instance().pushBackModel(MeshFactory::getInstance().createAxis());
    //debugRenderer->pushBackModel(MeshFactory::getInstance().createFrustum(corners));

    //m_renderables.push_back(std::make_unique<RenderModel>(m_deviceResources.get()));
    //m_renderables.push_back(std::make_unique<RenderFullscreenQuad>(m_deviceResources.get()));
    //m_renderables.push_back(std::make_unique<MengerRenderer>(m_deviceResources.get()));
    //m_renderables.push_back(std::make_unique<BakeModelParticles>(m_deviceResources.get()));
    //m_renderables.push_back(std::move(debugRenderer));

    m_cameraControllerFPS = std::make_unique<CameraControllerFPS>(m_deviceResources.get());

    m_sceneMenger = std::make_unique<DemoParticles::SceneMenger>(m_deviceResources.get());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.WantCaptureMouse = true;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{

    //TODO don't pass timer and pass elapsed time instead

    //float elapsedTime = float(timer.GetElapsedSeconds());

    m_inputManager->update();
    m_cameraControllerFPS->update(m_timer);

    // TODO: Replace this with your app's content update functions.
    for (auto& renderable : m_renderables)
    {
        renderable->update(m_timer, m_cameraControllerFPS->getCamera());
    }

    DebugRenderer::instance().update(timer, m_cameraControllerFPS->getCamera());

    m_sceneMenger->update(timer, m_cameraControllerFPS->getCamera());

    //m_fpsTextRenderer->Update(m_timer);

    //elapsedTime;
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    for (auto& renderable : m_renderables)
    {
        renderable->render();
    }

    DebugRenderer::instance().render();

    m_sceneMenger->render();

    //m_fpsTextRenderer->Render();

    m_deviceResources->PIXEndEvent();

    RenderImGui();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::Black /*Colors::CornflowerBlue*/);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    //width = 1024;
    //height = 720;

    width = 1600;
    height = 900;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    RenderStatesHelper::instance().init(m_deviceResources.get());

    DebugRenderer::instance().setDeviceResources(m_deviceResources.get());
    DebugRenderer::instance().createDeviceDependentResources();
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    DebugRenderer::instance().createWindowSizeDependentResources();
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}

void Game::RenderImGui()
{
    m_deviceResources->PIXBeginEvent(L"ImGui");
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    /*bool show_demo_window = true;
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    */
    ImGui::Begin("Particles globals");

    ImGui::Checkbox("Disable culling", &ParticlesGlobals::g_cullNone);
    const char* orientationItems[] = { "Billboard", "Backed Normal", "Direction" };
    ImGui::Combo("Particles orientation", &ParticlesGlobals::g_particlesOrientation, orientationItems, 3);
    const char* items[] = { "Opaque", "NonPremultiplied", "Additive" };
    ImGui::Combo("Blend Mode", &ParticlesGlobals::g_blendMode, items, 3);
    
    ImGui::End();

    // Rendering
    ImGui::Render();
    //g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    //g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_deviceResources->PIXEndEvent();
}

#pragma endregion