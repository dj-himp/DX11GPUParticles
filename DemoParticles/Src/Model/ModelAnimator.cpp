#include "pch.h"
#include "ModelAnimator.h"

#include "Bone.h"

#include <assimp/scene.h>

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ModelAnimator::ModelAnimator()
    {

    }

    ModelAnimator::~ModelAnimator() = default;

    bool ModelAnimator::init(const aiScene* scene)
    {
        if (!scene->HasAnimations()) {
            return false;
        }
        release();
        m_skeleton = createBoneTree(scene->mRootNode, nullptr);

        for(unsigned int i=0;i<scene->mNumMeshes;++i)
        {
            aiMesh* mesh = scene->mMeshes[i];
            for(unsigned int j=0;j<mesh->mNumBones;++j)
            {
                aiBone* bone = mesh->mBones[j];
                
                auto found = m_bonesByName.find(std::string(bone->mName.C_Str()));
                if (found == m_bonesByName.end()) continue;

                //bool skip = (from t in _bones let bname = bone.Name where t.Name == bname select t).Any();)
                //bool skip = std::find_if(m_bones.begin(), m_bones.end(), [&](Bone* b) { return b->m_name == std::string(bone->mName.C_Str()); }) != m_bones.end();
                bool skip = std::any_of(m_bones.begin(), m_bones.end(), [&](const Bone* b) { return b->m_name == std::string(bone->mName.C_Str()); });
                if (skip) continue;

                //found.Offset = Matrix.Transpose(bone.OffsetMatrix.ToMatrix());
                found->second->m_offset = AssimpToDX::FromMatrix(bone->mOffsetMatrix.Transpose());
                m_bones.push_back(found->second.get());
                //m_bonesToIndex[found->first] = m_bones.IndexOf(found);
                m_bonesToIndex[found->first] = (int)m_bones.size() - 1;
            }
            

            
            //NOT WORKING ??
            
            //b : all bones where mesh.bones.name != m_bonesByName.keys && startWith "Bone"
            /*foreach(var bone in _bonesByName.Keys.Where(b = > mesh1.Bones.All(b1 = > b1.Name != b) && b.StartsWith("Bone"))) {
                _bonesByName[bone].Offset = _bonesByName[bone].Parent.Offset;
                _bones.Add(_bonesByName[bone]);
                _bonesToIndex[bone] = _bones.IndexOf(_bonesByName[bone]);
            }*/
            
            /*for (const auto& entry : m_bonesByName)
            {
                const std::string& b = entry.first;
                Bone* boneInfo = entry.second.get();
                if (b.find("Bone") != 0)
                {
                    continue;
                }
                bool found = true;
                for (int j = 0; j < mesh->mNumBones; ++j)
                {
                    std::string b1 = std::string(mesh->mBones[j]->mName.C_Str());
                    if (b1 == b)
                    {
                        found = false;
                        break;
                    }
                }
                if(!found)
                    continue;

                boneInfo->m_offset = boneInfo->m_parent->m_offset;
                m_bones.push_back(boneInfo);
                m_bonesToIndex[b] = m_bones.size() - 1;
            }*/

            
        }
        extractAnimations(scene);
        const float timestep = 1.0f / 30.0f;
        for (int i = 0; i < m_animations.size(); ++i) {
            setAnimationIndex(i);
            float dt = 0.0f;
            for (float ticks = 0.0f; ticks < m_animations[i]->m_duration; ticks += m_animations[i]->m_ticksPerSecond / 30.0f) {
                dt += timestep;
                calculate(dt);
                std::vector<Matrix> trans;
                for (int a = 0; a < m_bones.size(); ++a) {
                    Matrix rotMat = m_bones[a]->m_offset * m_bones[a]->m_globalTransform;
                    trans.push_back(rotMat);
                }
                m_animations[i]->m_transforms.push_back(trans);
            }
        }

        return true;
    }


    bool ModelAnimator::setAnimation(std::string animation)
    {
        auto it = m_animationNameToId.find(animation);
        if (it != m_animationNameToId.end())
        {
            std::ptrdiff_t index = std::distance(m_animationNameToId.begin(), it);
            int oldIndex = m_currentAnimationIndex;
            m_currentAnimationIndex = (int)index;
            return oldIndex != m_currentAnimationIndex;
        }
        return false;
    }

    int ModelAnimator::getBoneIndex(std::string name)
    {
        
        if (m_bonesToIndex.find(name) != m_bonesToIndex.end())
        {
            return m_bonesToIndex[name];
        }
        
        return -1;
    }

    void ModelAnimator::release()
    {
        m_currentAnimationIndex = -1;
        m_animations.clear();
        m_skeleton = nullptr;
    }

    Bone* ModelAnimator::createBoneTree(aiNode* node, Bone* parent)
    {
        std::unique_ptr<Bone> internalNode = std::make_unique<Bone>();
        internalNode->m_name = std::string(node->mName.C_Str());
        internalNode->m_parent = parent;
                
        if (internalNode->m_name == "") {
            internalNode->m_name = std::string("foo") + std::to_string(m_i++);
        }

        
        aiMatrix4x4 trans = node->mTransformation;
        trans.Transpose();
        internalNode->m_localTransform = AssimpToDX::FromMatrix(trans);
        internalNode->m_originalLocalTransform = internalNode->m_localTransform;
        calculateBoneToWorldTransform(internalNode.get());

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            Bone* child = createBoneTree(node->mChildren[i], internalNode.get());
            if (child != nullptr)
            {
                internalNode->m_childrens.push_back(child);
            }
        }

        Bone* bone = internalNode.get();
        m_bonesByName[internalNode->m_name] = std::move(internalNode);

        return bone;
    }

    void ModelAnimator::calculateBoneToWorldTransform(Bone* child)
    {
        child->m_globalTransform = child->m_localTransform;
        Bone* parent = child->m_parent;
        while (parent != nullptr)
        {
            child->m_globalTransform *= parent->m_localTransform;
            parent = parent->m_parent;
        }
    }

    void ModelAnimator::extractAnimations(const aiScene* scene)
    {
        for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
        {
            m_animations.emplace_back(std::make_unique<AnimEvaluator>(scene->mAnimations[i]));
            m_animationNameToId[m_animations[i]->m_name] = i;
        }
        
        m_currentAnimationIndex = 0;
    }

    void ModelAnimator::setAnimationIndex(int animIndex)
    {
        if (animIndex >= m_animations.size()) {
            return;
        }

        m_currentAnimationIndex = animIndex;
    }

    void ModelAnimator::calculate(float dt)
    {
        if ((m_currentAnimationIndex < 0) || (m_currentAnimationIndex >= m_animations.size())) {
            return;
        }
        m_animations[m_currentAnimationIndex]->evaluate(dt, m_bonesByName);
        updateTransforms(m_skeleton);
    }

    void ModelAnimator::updateTransforms(Bone* node)
    {
        calculateBoneToWorldTransform(node);
        for(Bone* child : node->m_childrens) {
            updateTransforms(child);
        }
    }

}
