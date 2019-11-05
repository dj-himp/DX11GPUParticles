#pragma once

namespace DemoParticles
{
    static class ParticlesGlobals
    {
    public:
        //inline for static variable only on C++17
        static inline int g_particlesOrientation = 0;
        static inline bool g_cullNone = true;
        static inline int g_blendMode = 2;
    };

}
