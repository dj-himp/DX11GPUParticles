#pragma once
#include "ModelMesh.h"

#define MAX_NUM_BONES_PER_VERTEX 4 

struct aiNode;
struct aiScene;
struct aiAnimation;
struct aiNodeAnim;

namespace DemoParticles
{
    
    class Model
    {
    public:

        struct VertexBoneData
        {
            int m_boneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
            float m_weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };
            int m_index = 0;  // slot for the next update

            VertexBoneData()
            {
            }

            void AddBoneData(int boneID, float weight)
            {
                for (int i = 0; i < m_index; i++) {
                    if (m_boneIDs[i] == boneID) {
                        //  printf("bone %d already found at index %d old weight %f new weight %f\n", BoneID, i, Weights[i], Weight);
                        return;
                    }
                }

                // The iClone 7 Raptoid Mascot (https://sketchfab.com/3d-models/iclone-7-raptoid-mascot-free-download-56a3e10a73924843949ae7a9800c97c7)
                // has a problem of zero weights causing an overflow and the assertion below. This fixes it.
                if (weight == 0.0f) {
                    return;
                }

                // printf("Adding bone %d weight %f at index %i\n", BoneID, Weight, index);

                if (m_index == MAX_NUM_BONES_PER_VERTEX) {
                    return;
                    assert(0);
                }

                m_boneIDs[m_index] = boneID;
                m_weights[m_index] = weight;

                m_index++;
            }
        };

        struct BoneInfo
        {
            DirectX::SimpleMath::Matrix m_offsetMatrix;
            DirectX::SimpleMath::Matrix m_finalTransformation;

            BoneInfo(const DirectX::SimpleMath::Matrix offset)
            {
                m_offsetMatrix = offset;
                //FinalTransformation.SetZero();
            }
        };

        std::vector<BoneInfo> m_boneInfo;
        std::map<std::string, int> m_boneNameToIndexMap;
        std::vector<VertexBoneData> m_Bones;

    public:

        Model(const DX::DeviceResources* deviceResources);

        DirectX::SimpleMath::Vector3 getAaBoxMin() { return m_aaBoxMin; }
        void setAaBoxMin(DirectX::SimpleMath::Vector3 aaboxMin) { m_aaBoxMin = aaboxMin; }

        DirectX::SimpleMath::Vector3 getAaBoxMax() { return m_aaBoxMax; }
        void setAaBoxMax(DirectX::SimpleMath::Vector3 aaboxMax) { m_aaBoxMax = aaboxMax; }

        DirectX::SimpleMath::Vector3 getAaBoxCentre() { return m_aaBoxCentre; }
        void setAaBoxCentre(DirectX::SimpleMath::Vector3 aaboxCentre) { m_aaBoxCentre = aaboxCentre; }

        //std::vector<ShaderResourceView> getDiffuses();
        
        std::unique_ptr<ModelMesh>& AddMesh();
        void SetAABox(DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max);

        void GetBoneTransforms(float TimeInSeconds, std::vector<DirectX::SimpleMath::Matrix>& Transforms);
        void ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const DirectX::SimpleMath::Matrix& ParentTransform);
        const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName);
        void setInputElements(std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements) { m_inputElements = inputElements; }
        const std::vector<D3D11_INPUT_ELEMENT_DESC> getInputElements() { return m_inputElements; }

        void setVertexStride(size_t stride) { m_vertexStride = stride; }
        size_t getVertexStride() { return m_vertexStride; }

        const size_t getMeshCount() { return m_meshes.size(); }
        const std::unique_ptr<ModelMesh>& getMesh(int index) { return m_meshes[index]; }

        const aiScene* m_scene = nullptr;
        DirectX::SimpleMath::Matrix m_GlobalInverseTransform;
    private:
        const DX::DeviceResources* m_deviceResources;

        std::vector<std::unique_ptr<ModelMesh>> m_meshes;
        
        std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputElements;
        size_t m_vertexStride;

        DirectX::SimpleMath::Vector3 m_aaBoxMin;
        DirectX::SimpleMath::Vector3 m_aaBoxMax;
        DirectX::SimpleMath::Vector3 m_aaBoxCentre;

        
    };
}