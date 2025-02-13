#pragma once

namespace DemoParticles
{
    class ParticlesGlobals
    {
    public:
        //inline for static variable only on C++17
        static inline bool g_cullNone = true;
        static inline int g_blendMode = 2;
        static inline int g_particleShape = 0;

        static inline bool g_enableDetailDebug = true;

        static inline float g_emitterEmitRate = 1.0f / 60.0f;

        static inline int g_maxParticles = 20 * 1000 * 1024;// 1000 * 1024;
    };

}
