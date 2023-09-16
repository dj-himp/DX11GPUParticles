#pragma once

namespace DemoParticles
{
    class GpuProfiler
    {
    public:

        enum TimeStamp
        {
            TS_Emit,
            TS_Simulate,
            TS_Sort,
            TS_Render,

            TS_Max
        };
        static constexpr std::array s_enumNames {
            "Emit",
            "Simulate",
            "Sort",
            "Render"
        };

        static GpuProfiler& instance();

        void init(const DX::DeviceResources* deviceResources);

        void beginFrame();
        void beginTimestamp(TimeStamp ts);
        void endTimestamp(TimeStamp ts);
        void endFrame();

        void waitAndGetData(DX::StepTimer& timer);

        float getTimestamp(TimeStamp ts) { return m_lastFrameTimings[ts]; }
        float getTimestampAverage(TimeStamp ts) { return m_lastFrameTimingsAverage[ts]; }

    private:
        GpuProfiler() {}
        
        const DX::DeviceResources* m_deviceResources;

        int m_frameQuery = 0; //[double buffered] query beeing filled;
        int m_frameCollect = -1; //[double buffered] query to collect;

        Microsoft::WRL::ComPtr<ID3D11Query> m_queryTsDisjoint[2];
        Microsoft::WRL::ComPtr<ID3D11Query> m_queryTsBeginFrame[2];
        Microsoft::WRL::ComPtr<ID3D11Query> m_queryTsEndFrame[2];

        Microsoft::WRL::ComPtr<ID3D11Query> m_queryTsBegin[TS_Max][2];
        Microsoft::WRL::ComPtr<ID3D11Query> m_queryTsEnd[TS_Max][2];

        float m_lastFrameTimings[TS_Max];
        float m_lastFrameTimingsAverage[TS_Max]; //average to 0.5s

        float m_totalTimeDuringAverage[TS_Max];
        int m_frameCountDuringAverage;
        float m_timeBeginAverage = 0.0f;

        bool m_hasBegun = false; //when activating the debug to be sure to have a begin before the end
    };
}
