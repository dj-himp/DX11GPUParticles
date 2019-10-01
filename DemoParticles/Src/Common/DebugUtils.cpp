#include "pch.h"
#include "DebugUtils.h"

namespace DemoParticles
{
    void DemoParticles::DebugUtils::log(std::string str)
    {
        str += "\n";
        OutputDebugStringA(str.c_str());
    }
}
