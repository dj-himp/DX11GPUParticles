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
        
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTsBeginFrame[0])
        );

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTsEndFrame[0])
        );

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTsBeginFrame[1])
        );

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTsEndFrame[1])
        );

        for (TimeStamp t = TimeStamp(0); t < TS_Max; t = TimeStamp(t+1))
        {
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTsBegin[t][0])
            );

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTsBegin[t][1])
            );

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTsEnd[t][0])
            );

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateQuery(&queryDesc, &m_queryTsEnd[t][1])
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
        m_deviceResources->GetD3DDeviceContext()->End(m_queryTsBeginFrame[m_frameQuery].Get());
    }

    void GpuProfiler::beginTimestamp(TimeStamp ts)
    {
        if (!m_hasBegun) return;
        
        m_deviceResources->GetD3DDeviceContext()->End(m_queryTsBegin[ts][m_frameQuery].Get());
    }

    void GpuProfiler::endTimestamp(TimeStamp ts)
    {
        if (!m_hasBegun) return;

        m_deviceResources->GetD3DDeviceContext()->End(m_queryTsEnd[ts][m_frameQuery].Get());
    }

    void GpuProfiler::endFrame()
    {
        if (!m_hasBegun) return;

        m_deviceResources->GetD3DDeviceContext()->End(m_queryTsEndFrame[m_frameQuery].Get());
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

        UINT64 TimestampBeginFrame;
        if (context->GetData(m_queryTsBeginFrame[iFrame].Get(), &TimestampBeginFrame, sizeof(UINT64), 0) != S_OK)
        {
            std::string s = "[GPU PROFILER] can't get data for begin FRame: ";
            //s += std::to_string(TS_BeginFrame);
            DebugUtils::log(s);
            return;
        }

        for (TimeStamp t = TimeStamp(0); t < TS_Max; t = TimeStamp(t + 1))
        {
            UINT64 timestampBegin;
            if (context->GetData(m_queryTsBegin[t][iFrame].Get(), &timestampBegin, sizeof(UINT64), 0) != 0)
            {
                std::string s = "[GPU PROFILER] can't get data for begin : ";
                s += std::to_string(t);
                DebugUtils::log(s);
                return;
            }

            UINT64 timestampEnd;
            if (context->GetData(m_queryTsEnd[t][iFrame].Get(), &timestampEnd, sizeof(UINT64), 0) != 0)
            {
                std::string s = "[GPU PROFILER] can't get data for end : ";
                s += std::to_string(t);
                DebugUtils::log(s);
                return;
            }

            m_lastFrameTimings[t] = float(timestampEnd - timestampBegin) / float(timestampDisjointData.Frequency);

            m_totalTimeDuringAverage[t] += m_lastFrameTimings[t];
        }

        UINT64 TimestampEndFrame;
        if (context->GetData(m_queryTsEndFrame[iFrame].Get(), &TimestampEndFrame, sizeof(UINT64), 0) != S_OK)
        {
            std::string s = "[GPU PROFILER] can't get data for endFrame: ";
            //s += std::to_string(TS_BeginFrame);
            DebugUtils::log(s);
            return;
        }

        m_frameCountDuringAverage++;
        if (timer.GetTotalSeconds() > m_timeBeginAverage + 0.5f)
        {
            for (TimeStamp t = TimeStamp(0); t < TS_Max; t = TimeStamp(t + 1))
            {
                m_lastFrameTimingsAverage[t] = m_totalTimeDuringAverage[t] / m_frameCountDuringAverage;
                m_totalTimeDuringAverage[t] = 0.0f;
            }
            m_frameCountDuringAverage = 0;
            m_timeBeginAverage = (float)timer.GetTotalSeconds();
        }
    }

}
