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
        m_deviceResources->GetD3DDeviceContext()->Begin(m_queryTsDisjoint[m_frameQuery].Get());
        setTimestamp(TS_BeginFrame);
    }

    void GpuProfiler::setTimestamp(TimeStamp ts)
    {
        m_deviceResources->GetD3DDeviceContext()->End(m_queryTs[ts][m_frameQuery].Get());
    }

    void GpuProfiler::endFrame()
    {
        setTimestamp(TS_EndFrame);
        m_deviceResources->GetD3DDeviceContext()->End(m_queryTsDisjoint[m_frameQuery].Get());

        m_frameQuery = (m_frameCollect + 1) % 2;
    }

    void GpuProfiler::waitAndGetData()
    {
        //wait one frame before collect
        if (m_frameCollect < 0)
        {
            m_frameCollect = 0;
            return;
        }

        auto context = m_deviceResources->GetD3DDeviceContext();
        while (context->GetData(m_queryTsDisjoint[m_frameCollect].Get(), nullptr, 0, 0) == S_FALSE)
        {
            Sleep(1);
        }

        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timestampDisjointData;
        if (context->GetData(m_queryTsDisjoint[m_frameCollect].Get(), &timestampDisjointData, sizeof(timestampDisjointData), 0) != S_OK)
        {
            DebugUtils::log("[GPU PROFILER] failed to get disjoint data");
            return;
        }

        if (timestampDisjointData.Disjoint)
        {
            DebugUtils::log("[GPU PROFILER] data disjoint");
        }

        UINT64 previousTimestamp;
        if (context->GetData(m_queryTs[TS_BeginFrame][m_frameCollect].Get(), &previousTimestamp, sizeof(UINT64), 0) != S_OK)
        {
            std::string s = "[GPU PROFILER] can't get data for : ";
            s += std::to_string(TS_BeginFrame);
            DebugUtils::log(s);
            return;
        }

        for (TimeStamp t = TS_BeginFrame; t < TS_Max; t = TimeStamp(t + 1))
        {
            UINT64 timestamp;
            if (context->GetData(m_queryTs[t][m_frameCollect].Get(), &timestamp, sizeof(UINT64), 0) != 0)
            {
                std::string s = "[GPU PROFILER] can't get data for : ";
                s += std::to_string(t);
                DebugUtils::log(s);
                return;
            }

            m_lastFrameTimings[t] = float(timestamp - previousTimestamp) / float(timestampDisjointData.Frequency);
            previousTimestamp = timestamp;

            //m_totalTimeDuringAverage[t] += m_lastFrameTimings[t];
        }

        //TODO AVERAGE
        //m_frameCountDuringAverage++;
        //if()

        m_frameCollect = (m_frameCollect + 1) % 2;
    }

}
