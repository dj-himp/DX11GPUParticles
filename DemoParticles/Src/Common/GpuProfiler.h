#pragma once

namespace DemoParticles
{
    class GpuProfiler
    {
    public:

        enum TimeStamp
        {
            TS_BeginFrame,

            TS_inter,

            TS_EndFrame,

            TS_Max
        };

        static GpuProfiler& instance();

        void init(const DX::DeviceResources* deviceResources);

        void beginFrame();
        void setTimestamp(TimeStamp ts);
        void endFrame();

        void waitAndGetData();

        float getTimestamp(TimeStamp ts) { return m_lastFrameTimings[ts]; }

    private:
        GpuProfiler() {}
        
        const DX::DeviceResources* m_deviceResources;

        int m_frameQuery = 0; //[double buffered] query beeing filled;
        int m_frameCollect = -1; //[double buffered] query to collect;

        Microsoft::WRL::ComPtr<ID3D11Query> m_queryTsDisjoint[2];
        Microsoft::WRL::ComPtr<ID3D11Query> m_queryTs[TS_Max][2];

        float m_lastFrameTimings[TS_Max];
        float m_lastFrameTimingsAverage[TS_Max]; //average to 0.5s

        float m_totalTimeDuringAverage[TS_Max];
        int m_frameCountDuringAverage;
        float m_timeBeginAverage;
    };
}
