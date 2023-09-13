#include "pch.h"
#include "GpuProfiler.h"

namespace DemoParticles
{
    GpuProfiler& GpuProfiler::instance()
    {
        static GpuProfiler instance = GpuProfiler();
        return instance;
    }

    void GpuProfiler::init(const DX::DeviceResources* deviceResources)
    {
        m_deviceResources = deviceResources;

        D3D11_QUERY_DESC queryDesc;
        ZeroMemory(&queryDesc, sizeof(queryDesc));

        queryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
        
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTsDisjoint[0])
        );

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTsDisjoint[1])
        );

        queryDesc.Query = D3D11_QUERY_TIMESTAMP;

        for (TimeStamp t = TS_BeginFrame; t < TS_Max; t = TimeStamp(t+1))
        {
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTs[t][0])
            );

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTs[t][1])
            );
        }

    }

    void GpuProfiler::beginFrame()
    {
        if (ParticlesGlobals::g_enableDetailDebug == false)
        {
            m_hasBegun = false;
            return;
        }
        else
        {
            m_hasBegun = true;
        }

        m_deviceResources->GetD3DDeviceContext()->Begin(m_queryTsDisjoint[m_frameQuery].Get());
        setTimestamp(TS_BeginFrame);
    }

    void GpuProfiler::setTimestamp(TimeStamp ts)
    {
        if (!m_hasBegun) return;

        m_deviceResources->GetD3DDeviceContext()->End(m_queryTs[ts][m_frameQuery].Get());
    }

    void GpuProfiler::endFrame()
    {
        if (!m_hasBegun) return;

        setTimestamp(TS_EndFrame);
        m_deviceResources->GetD3DDeviceContext()->End(m_queryTsDisjoint[m_frameQuery].Get());


        ++m_frameQuery &= 1;
    }

    void GpuProfiler::waitAndGetData(DX::StepTimer& timer)
    {
        if (!m_hasBegun) return;

        //wait one frame before collect
        if (m_frameCollect < 0)
        {
            m_frameCollect = 0;
            return;
        }

        auto context = m_deviceResources->GetD3DDeviceContext();
        while (context->GetData(m_queryTsDisjoint[m_frameCollect].Get(), nullptr, 0, 0) == S_FALSE)
        {
            //don't sleep : make fps drop
            //Sleep(1);
        }

        int iFrame = m_frameCollect;
        ++m_frameCollect &= 1;

        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timestampDisjointData;
        if (context->GetData(m_queryTsDisjoint[iFrame].Get(), &timestampDisjointData, sizeof(timestampDisjointData), 0) != S_OK)
        {
            DebugUtils::log("[GPU PROFILER] failed to get disjoint data");
            return;
        }

        if (timestampDisjointData.Disjoint)
        {
            DebugUtils::log("[GPU PROFILER] data disjoint");
        }

        UINT64 previousTimestamp;
        if (context->GetData(m_queryTs[TS_BeginFrame][iFrame].Get(), &previousTimestamp, sizeof(UINT64), 0) != S_OK)
        {
            std::string s = "[GPU PROFILER] can't get data for : ";
            s += std::to_string(TS_BeginFrame);
            DebugUtils::log(s);
            return;
        }

        for (TimeStamp t = TimeStamp(TS_BeginFrame + 1); t < TS_Max; t = TimeStamp(t + 1))
        {
            UINT64 timestamp;
            if (context->GetData(m_queryTs[t][iFrame].Get(), &timestamp, sizeof(UINT64), 0) != 0)
            {
                std::string s = "[GPU PROFILER] can't get data for : ";
                s += std::to_string(t);
                DebugUtils::log(s);
                return;
            }

            m_lastFrameTimings[t] = float(timestamp - previousTimestamp) / float(timestampDisjointData.Frequency);
            previousTimestamp = timestamp;

            m_totalTimeDuringAverage[t] += m_lastFrameTimings[t];
        }

        m_frameCountDuringAverage++;
        if (timer.GetTotalSeconds() > m_timeBeginAverage + 0.5f)
        {
            for (TimeStamp t = TS_BeginFrame; t < TS_Max; t = TimeStamp(t + 1))
            {
                m_lastFrameTimingsAverage[t] = m_totalTimeDuringAverage[t] / m_frameCountDuringAverage;
                m_totalTimeDuringAverage[t] = 0.0f;
            }
            m_frameCountDuringAverage = 0;
            m_timeBeginAverage = timer.GetTotalSeconds();
        }
    }

}
