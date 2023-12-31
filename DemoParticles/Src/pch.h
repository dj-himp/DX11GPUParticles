//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <winsdkver.h>
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl/client.h>

#include <d3d11_1.h>

#if defined(NTDDI_WIN10_RS2)
#include <dxgi1_6.h>
#else
#include <dxgi1_5.h>
#endif

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>
#include <array>

#include <stdio.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <d2d1_3.h>
#include <d3dcompiler.h>

#include <iostream>
#include <vector>
#include "simpleMath.h"
#include "Model.h"
#include "Effects.h"
#include "CommonStates.h"
#include "mouse.h"
#include "keyboard.h"
#include "Common/DebugUtils.h"
#include "Common/DeviceResources.h"
#include "Common/StepTimer.h"
#include "Common/RenderStatesHelper.h"
#include "GlobalSettings.h"
#include "Common/DirectXHelper.h"
#include "Common/GpuProfiler.h"

#include "imgui/imgui.h"
#include "Gui/imgui_impl_win32.h"
#include "Gui/imgui_impl_dx11.h"
#include "imgui/ImGuizmo.h"
#include "imgui/ImSequencer.h"
#include "imgui/ImCurveEdit.h"
#include "Json/json.hpp"
#include "Gui/Sequencer.h"
#include "choreograph/Choreograph.h"
#include "Model/AssimpToDX.h"

#include <DDSTextureLoader.h>

namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) : result(hr) {}

        virtual const char* what() const override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
            return s_str;
        }

    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }
}