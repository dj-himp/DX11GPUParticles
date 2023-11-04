#include "pch.h"
#include "AnimEvaluator.h"

#include "Bone.h"

#include <assimp/anim.h>

namespace DemoParticles
{
    
    AnimEvaluator::AnimEvaluator(aiAnimation* anim)
    {
        m_lastTime = 0.0f;
        m_ticksPerSecond = anim->mTicksPerSecond > 0.0f ? (float)anim->mTicksPerSecond : 920.0f;
        m_duration = (float)anim->mDuration;
        m_name = std::string(anim->mName.C_Str());

        for (int i = 0; i < anim->mNumChannels; ++i)
        {
            AnimationChannel chan;
            chan.m_name = std::string(anim->mChannels[i]->mNodeName.C_Str());

            for (int j = 0; j < anim->mChannels[i]->mNumPositionKeys; ++j)
            {
                chan.m_positionKeys.push_back(anim->mChannels[i]->mPositionKeys[j]);
            }

            for (int j = 0; j < anim->mChannels[i]->mNumRotationKeys; ++j)
            {
                chan.m_rotationKeys.push_back(anim->mChannels[i]->mRotationKeys[j]);
            }

            for (int j = 0; j < anim->mChannels[i]->mNumScalingKeys; ++j)
            {
                chan.m_scalingKeys.push_back(anim->mChannels[i]->mScalingKeys[j]);
            }
            
            m_channels.push_back(chan);
        }

        m_lastPositions.resize(anim->mNumChannels);
        m_playAnimationForward = true;
    }

    void AnimEvaluator::evaluate(float dt, std::map<std::string, std::unique_ptr<Bone>>& bonesByName)
    {
        dt *= m_ticksPerSecond;
        float time = 0.0f;
        if (m_duration > 0.0f)
        {
            time = fmod(dt, m_duration);
        }
        for (int i = 0; i < m_channels.size(); ++i)
        {
            AnimationChannel channel = m_channels[i];
            if (bonesByName.find(channel.m_name) == bonesByName.end())
            {
                //Console.WriteLine("Did not find the bone node " + channel.Name);
                continue;
            }
            // interpolate position keyframes
            aiVector3D position;
            if (channel.m_positionKeys.size() > 0) {
                int frame = (time >= m_lastTime) ? std::get<0>(m_lastPositions[i]) : 0;
                while (frame < channel.m_positionKeys.size() - 1)
                {
                    if (time < channel.m_positionKeys[frame + 1].mTime)
                    {
                        break;
                    }
                    frame++;
                }
                if (frame >= channel.m_positionKeys.size())
                {
                    frame = 0;
                }

                int nextFrame = (frame + 1) % channel.m_positionKeys.size();

                aiVectorKey key = channel.m_positionKeys[frame];
                aiVectorKey nextKey = channel.m_positionKeys[nextFrame];
                float diffTime = nextKey.mTime - key.mTime;
                if (diffTime < 0.0) {
                    diffTime += m_duration;
                }
                if (diffTime > 0.0) {
                    float factor = (time - key.mTime) / diffTime;
                    position = key.mValue + (nextKey.mValue - key.mValue) * factor;
                }
                else
                {
                    position = key.mValue;
                }
                std::get<0>(m_lastPositions[i]) = frame;

            }
            // interpolate rotation keyframes
            aiQuaternion pRot = aiQuaternion(1, 0, 0, 0);
            if (channel.m_rotationKeys.size() > 0)
            {
                int frame = (time >= m_lastTime) ? std::get<1>(m_lastPositions[i]) : 0;
                while (frame < channel.m_rotationKeys.size() - 1)
                {
                    if (time < channel.m_rotationKeys[frame + 1].mTime)
                    {
                        break;
                    }
                    frame++;
                }
                if (frame >= channel.m_rotationKeys.size())
                {
                    frame = 0;
                }
                int nextFrame = (frame + 1) % channel.m_rotationKeys.size();

                aiQuatKey key = channel.m_rotationKeys[frame];
                aiQuatKey nextKey = channel.m_rotationKeys[nextFrame];
                key.mValue.Normalize();
                nextKey.mValue.Normalize();
                float diffTime = nextKey.mTime - key.mTime;
                if (diffTime < 0.0)
                {
                    diffTime += m_duration;
                }
                if (diffTime > 0)
                {
                    float factor = (float)((time - key.mTime) / diffTime);
                    aiQuaternion::Interpolate(pRot, key.mValue, nextKey.mValue, factor);
                }
                else {
                    pRot = key.mValue;
                }
                std::get<1>(m_lastPositions[i]) = frame;

            }
            // interpolate scale keyframes
            aiVector3D pscale = aiVector3D(1);
            if (channel.m_scalingKeys.size() > 0)
            {
                float frame = (time >= m_lastTime) ? std::get<2>(m_lastPositions[i]) : 0;
                while (frame < channel.m_scalingKeys.size() - 1)
                {
                    if (time < channel.m_scalingKeys[frame + 1].mTime)
                    {
                        break;
                    }
                    frame++;
                }
                if (frame >= channel.m_scalingKeys.size())
                {
                    frame = 0;
                }
                std::get<2>(m_lastPositions[i]) = frame;
            }

            // create the combined transformation matrix
            aiMatrix4x4 mat = aiMatrix4x4(pRot.GetMatrix());
            mat.a1 *= pscale.x; mat.b1 *= pscale.x; mat.c1 *= pscale.x;
            mat.a2 *= pscale.y; mat.b2 *= pscale.y; mat.c2 *= pscale.y;
            mat.a3 *= pscale.z; mat.b3 *= pscale.z; mat.c3 *= pscale.z;
            mat.a4 = position.x; mat.b4 = position.y; mat.c4 = position.z;

            // transpose to get DirectX style matrix
            mat.Transpose();
            bonesByName[channel.m_name]->m_localTransform = AssimpToDX::FromMatrix(mat);
        }
        m_lastTime = time;
    }

    int AnimEvaluator::getFrameIndexAt(float dt)
    {
        dt *= m_ticksPerSecond;
        float time = 0.0f;
        if (m_duration > 0.0f) {
            time = fmod(dt, m_duration);
        }
        float percent = time / m_duration;
        if (!m_playAnimationForward) {
            percent = (percent - 1.0f) * -1.0f;
        }
        int frameIndexAt = (int)(m_transforms.size() * percent);
        return frameIndexAt;
    }

}