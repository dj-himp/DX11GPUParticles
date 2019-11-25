#pragma once

namespace DemoParticles
{
    static class ParticlesGlobals
    {
    public:
        //inline for static variable only on C++17
        static inline int g_particlesOrientation = 2;
        static inline bool g_cullNone = true;
        static inline int g_blendMode = 2;
        static inline float g_particlesColor[4] = { 0.5f, 0.2f, 0.2f, 1.0f };
    };

}
