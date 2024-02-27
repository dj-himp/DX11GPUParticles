#pragma once

#define MAX_NUM_BONES_PER_VERTEX 4

namespace DemoParticles
{
    struct Bone
    {
        unsigned int m_boneID[MAX_NUM_BONES_PER_VERTEX] = { 0 };
        float m_weight[MAX_NUM_BONES_PER_VERTEX] = { 0.0 };

        void AddBoneData(unsigned int boneID, float weight)
        {
            for (unsigned int i = 0; i < MAX_NUM_BONES_PER_VERTEX; ++i)
            {
                if (m_weight[i] == 0)
                {
                    m_boneID[i] = boneID;
                    m_weight[i] = weight;
                    return;
                }
            }
        }
    }
}