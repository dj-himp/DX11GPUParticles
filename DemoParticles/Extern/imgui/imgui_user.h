#pragma once

namespace ImGui
{
    enum class CurveEditorFlags
    {
        NO_TANGENTS = 1 << 0,
        SHOW_GRID = 1 << 1,
        RESET = 1 << 2
    };

    IMGUI_API int CurveEditor(const char* label
        , float* values
        , int points_count
        , const ImVec2& size = ImVec2(-1, -1)
        , ImU32 flags = 0
        , int* new_count = nullptr
        , int* selected_point = nullptr);
}