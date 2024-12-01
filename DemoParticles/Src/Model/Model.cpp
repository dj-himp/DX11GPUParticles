#include "pch.h"
#include "Model.h"

#include <assimp/scene.h>

namespace DemoParticles
{
    Model::Model(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {

    }

    //AssimpModel::~AssimpModel() = default;

    /*void AssimpModel::AddMesh(std::unique_ptr<ModelMesh> mesh)
    {
        m_meshes.push_back(mesh);
    }*/

    std::unique_ptr<ModelMesh>& Model::AddMesh()
    {
        std::unique_ptr<ModelMesh> mesh = std::make_unique<ModelMesh>(m_deviceResources);
        m_meshes.push_back(std::move(mesh));

        return m_meshes.back();
    }

    void Model::SetAABox(DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max)
    {
        m_aaBoxMin = min;
        m_aaBoxMax = max;
        m_aaBoxCentre = 0.5f * (min + max);
    }


	void Model::GetBoneTransforms(float TimeInSeconds, std::vector<DirectX::SimpleMath::Matrix>& Transforms)
	{
		Transforms.resize(m_boneInfo.size());

		DirectX::SimpleMath::Matrix Identity = DirectX::SimpleMath::Matrix::Identity;
		
		float TicksPerSecond = (float)(m_scene->mAnimations[0]->mTicksPerSecond != 0 ? m_scene->mAnimations[0]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSeconds * TicksPerSecond;
		float AnimationTimeTicks = fmod(TimeInTicks, (float)m_scene->mAnimations[0]->mDuration);

		ReadNodeHierarchy(AnimationTimeTicks, m_scene->mRootNode, Identity);

		for (int i = 0; i < m_boneInfo.size(); i++) {
			Transforms[i] = m_boneInfo[i].m_finalTransformation;
		}
	}

	int FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumPositionKeys == 1) {
			Out = pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		int PositionIndex = FindPosition(AnimationTimeTicks, pNodeAnim);
		int NextPositionIndex = PositionIndex + 1;
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
		float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - t1) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}


	int FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumRotationKeys > 0);

		for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1) {
			Out = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		int RotationIndex = FindRotation(AnimationTimeTicks, pNodeAnim);
		int NextRotationIndex = RotationIndex + 1;
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
		float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - t1) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out.Normalize();
	}


	int FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumScalingKeys > 0);

		for (int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumScalingKeys == 1) {
			Out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		int ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
		int NextScalingIndex = ScalingIndex + 1;
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
		float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}

	void Model::ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* node, const DirectX::SimpleMath::Matrix& ParentTransform)
	{
		std::string NodeName(node->mName.data);

		const aiAnimation* pAnimation = m_scene->mAnimations[0];

		DirectX::SimpleMath::Matrix NodeTransformation = AssimpToDX::FromMatrix(node->mTransformation);

		const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

		if (pNodeAnim) {
			// Interpolate scaling and generate scaling transformation matrix
			aiVector3D Scaling;
			CalcInterpolatedScaling(Scaling, AnimationTimeTicks, pNodeAnim);
			DirectX::SimpleMath::Matrix ScalingM;
			DirectX::SimpleMath::Matrix::CreateScale(Scaling.x, Scaling.y, Scaling.z);

			// Interpolate rotation and generate rotation transformation matrix
			aiQuaternion RotationQ;
			CalcInterpolatedRotation(RotationQ, AnimationTimeTicks, pNodeAnim);
			DirectX::SimpleMath::Matrix RotationM = DirectX::SimpleMath::Matrix::CreateFromQuaternion(DirectX::SimpleMath::Quaternion(RotationQ.x, RotationQ.y, RotationQ.z, RotationQ.w));

			// Interpolate translation and generate translation transformation matrix
			aiVector3D Translation;
			CalcInterpolatedPosition(Translation, AnimationTimeTicks, pNodeAnim);
			DirectX::SimpleMath::Matrix TranslationM = DirectX::SimpleMath::Matrix::CreateTranslation(Translation.x, Translation.y, Translation.z);
			

			// Combine the above transformations
			//NodeTransformation = TranslationM * RotationM * ScalingM;
			NodeTransformation = ScalingM * RotationM *TranslationM;
		}
		

		DirectX::SimpleMath::Matrix GlobalTransformation = ParentTransform * NodeTransformation;

		if (m_boneNameToIndexMap.find(NodeName) != m_boneNameToIndexMap.end()) {
			int BoneIndex = m_boneNameToIndexMap[NodeName];
			//m_boneInfo[BoneIndex].m_finalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_boneInfo[BoneIndex].m_offsetMatrix;
			m_boneInfo[BoneIndex].m_finalTransformation = m_boneInfo[BoneIndex].m_offsetMatrix * GlobalTransformation * m_GlobalInverseTransform;
		}

		for (int i = 0; i < node->mNumChildren; i++) {
			ReadNodeHierarchy(AnimationTimeTicks, node->mChildren[i], GlobalTransformation);
		}
	}

	const aiNodeAnim* Model::FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName)
	{
		for (int i = 0; i < pAnimation->mNumChannels; i++) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}

		return NULL;
	}
}
