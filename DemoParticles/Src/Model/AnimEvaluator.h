#pragma once

struct aiAnimation;
struct aiVectorKey;
struct aiQuatKey;
namespace DemoParticles
{   
    class Bone;

    class AnimEvaluator
    {
        friend class ModelAnimator;
    public:
        AnimEvaluator(aiAnimation* anim);
        ~AnimEvaluator() = default;

        void evaluate(float dt, std::map<std::string, std::unique_ptr<Bone>>& bones);
        std::vector<DirectX::SimpleMath::Matrix> getTransforms(float dt) { return m_transforms[getFrameIndexAt(dt)]; }

        std::vector<std::vector<DirectX::SimpleMath::Matrix>> m_transforms;

    private:

        struct AnimationChannel
        {
            std::string m_name;
            std::vector<aiVectorKey> m_positionKeys;
            std::vector<aiQuatKey> m_rotationKeys;
            std::vector<aiVectorKey> m_scalingKeys;
        };

        int getFrameIndexAt(float dt);

        std::string m_name;
        std::vector<AnimationChannel> m_channels;
        bool m_playAnimationForward;
        float m_lastTime;
        float m_ticksPerSecond;
        float m_duration;
        std::vector<std::tuple<int, int, int>> m_lastPositions;
        
    };
}