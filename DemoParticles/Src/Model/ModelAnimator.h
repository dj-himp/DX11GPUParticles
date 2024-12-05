#pragma once

#include "AnimEvaluator.h"

struct aiScene;
struct aiNode;

namespace DemoParticles
{    
    class Bone;

    class ModelAnimator
    {
       
    public:

        ModelAnimator();
        ~ModelAnimator();

        bool init(const aiScene* scene);

        bool hasSkeleton() { return m_bones.size(); }
        std::string animationName() { return m_animations[m_currentAnimationIndex]->m_name; }
        float getAnimationSpeed() { return m_animations[m_currentAnimationIndex]->m_ticksPerSecond; }
        float getDuration() { return m_animations[m_currentAnimationIndex]->m_duration / m_animations[m_currentAnimationIndex]->m_ticksPerSecond; }
        bool setAnimation(std::string animation);
        int getBoneIndex(std::string name);

        void playAnimationForward() { m_animations[m_currentAnimationIndex]->m_playAnimationForward = true; }
        void playAnimationBackward() { m_animations[m_currentAnimationIndex]->m_playAnimationForward = false; }
        void adjustAnimationSpeedBy(float prc) { m_animations[m_currentAnimationIndex]->m_ticksPerSecond *= prc; }
        void adjustAnimationSpeedTo(float ticksPerSec) { m_animations[m_currentAnimationIndex]->m_ticksPerSecond = ticksPerSec; }

        std::vector<DirectX::SimpleMath::Matrix> getTransforms(float dt) { return m_animations[m_currentAnimationIndex]->getTransforms(dt); }

        void setAnimationIndex(int animIndex);
    private:
        void release();

        
        Bone* createBoneTree(aiNode* node, Bone* parent);
        void calculateBoneToWorldTransform(Bone* child);
        void extractAnimations(const aiScene* scene);
        //void setAnimationIndex(int animIndex);
        void calculate(float dt);
        void updateTransforms(Bone* node);
        

        Bone* m_skeleton = nullptr;
        typedef std::map<std::string, std::unique_ptr<Bone>> BoneMap;
        BoneMap m_bonesByName;
        std::map<std::string, int> m_bonesToIndex;
        std::map<std::string, int> m_animationNameToId;
        std::vector<Bone*> m_bones;
        std::vector< std::unique_ptr<AnimEvaluator>> m_animations;
        int m_currentAnimationIndex = -1;
        int m_i;
        

    };
}