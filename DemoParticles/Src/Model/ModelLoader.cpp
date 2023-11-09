#include "pch.h"
#include "ModelLoader.h"

#include "Model.h"
#include "ModelMesh.h"
#include "..\Content\ShaderStructures.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ModelLoader::ModelLoader(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {

    }

    std::unique_ptr<DemoParticles::Model> ModelLoader::load(const std::string fileName, const bool createSRV)
    {
        std::unique_ptr<Model> model = std::make_unique<Model>(m_deviceResources);

        //doc said to convert to left handed in the importer if using directX (need to investigate)
        //answer : DirectX convention is left handed but as DirectTK is right handed living it like that
        const aiScene* scene = m_importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality /*aiProcessPreset_TargetRealtime_Fast*/);

        if(!scene)
        {
            std::string error = m_importer.GetErrorString();
            std::cerr << "[ModelLoader] Failed to load " << fileName << " with error : " << m_importer.GetErrorString() << "\n";
            assert(0);
        }

        loadMeshes(model, scene, createSRV);


        return std::move(model);
    }

    //Create meshes and add vertex and index buffers
    void ModelLoader::loadMeshes(std::unique_ptr<Model>& model, const aiScene* scene, const bool createSRV)
    {
        int baseVertex = 0;

        for (unsigned int meshId = 0; meshId < scene->mNumMeshes; ++meshId)
        {
            //get a mesh from the scene
            aiMesh* mesh = scene->mMeshes[meshId];

            //create new mesh to add to model
            std::unique_ptr<ModelMesh>& modelMesh = model->AddMesh();

            loadMeshBones(model, mesh);

            //if mesh has a material extract the diffuse texture, if present
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            if (material != nullptr && material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                aiString texturePath;
                material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
                //create new texture for mesh

                //original version with texture side by side with the model
                //modelMesh.AddTextureDiffuse(device, m_modelPath + "\\" + texture.FilePath);

                //Custom version to load texture from Textures folder
                //modelMesh.AddTextureDiffuse(/*"Textures" + "\\" + texture.FilePath*/std::string(texturePath.C_Str()));

            }

            //determine the elements in the vertex
            bool hasTexCoords = mesh->HasTextureCoords(0);
            bool hasColors = mesh->HasVertexColors(0);
            bool hasNormals = mesh->HasNormals();
            bool hasTangents = mesh->HasTangentsAndBitangents();
            bool hasBitangents = hasTangents;

            //create vertex element list 
            /*static const D3D11_INPUT_ELEMENT_DESC vertexElements[] = 
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UINT , 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };*/

            std::vector<D3D11_INPUT_ELEMENT_DESC> vertexElements;

            vertexElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
            UINT vertexSize = sizeof(Vector3);

            vertexElements.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT , 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 });
            vertexSize += sizeof(Color);

            vertexElements.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 });
            vertexSize += sizeof(Vector3);
 
            vertexElements.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 });
            vertexSize += sizeof(Vector3);
 
            vertexElements.push_back({ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 });
            vertexSize += sizeof(Vector3);
           
            vertexElements.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 });
            vertexSize += sizeof(Vector2);

            model->setInputElements(vertexElements);
            model->setVertexStride(sizeof(VertexObject));

            //set the vertex elements and size
            modelMesh->setInputElements(vertexElements);
            modelMesh->setVertexSize(vertexSize);

            //get pointers to vertex data
            aiVector3D* positions = mesh->mVertices;
            aiVector3D* texCoords = mesh->mTextureCoords[0];
            aiVector3D* normals = mesh->mNormals;
            aiVector3D* tangents = mesh->mTangents;
            aiVector3D* biTangents = mesh->mBitangents;
            //aiColor4D* colors = mesh->mColors[0];

            //also determine primitive type
            switch (mesh->mPrimitiveTypes)
            {
            case aiPrimitiveType_POINT:
                modelMesh->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
                break;
            case aiPrimitiveType_LINE:
                modelMesh->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
                break;
            case aiPrimitiveType_TRIANGLE:
                modelMesh->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                break;
            default:
                //ModelLoader::AddVertexData(): Unknown primitive type
                assert(0);
            }

            modelMesh->setBaseVertex(baseVertex);
            baseVertex += mesh->mNumVertices;

            //create data stream for vertices
            std::vector<VertexObject> vertices(mesh->mNumVertices);
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                //add position, after transforming it with accumulated node transform
                {
                    vertices[i].position = FromVector(positions[i]);
                }

                if (hasColors)
                {
                    vertices[i].color = FromColor(mesh->mColors[0][i]);
                }
                if (hasNormals)
                {
                    vertices[i].normal = FromVector(normals[i]);
                }
                if (hasTangents)
                {
                    vertices[i].tangent = FromVector(tangents[i]);
                }
                if (hasBitangents)
                {
                    vertices[i].bitangent = FromVector(biTangents[i]);
                }
                if (hasTexCoords)
                {
                    vertices[i].uv = Vector2(texCoords[i].x, 1 - texCoords[i].y);
                }
            }

            D3D11_BUFFER_DESC vertexBufferDesc;
            vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
            vertexBufferDesc.ByteWidth = sizeof(VertexObject) * mesh->mNumVertices;
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vertexBufferDesc.CPUAccessFlags = 0;
            vertexBufferDesc.MiscFlags = 0;

            D3D11_SUBRESOURCE_DATA vertexBufferData;
            vertexBufferData.pSysMem = &vertices[0];
            vertexBufferData.SysMemPitch = 0;
            vertexBufferData.SysMemSlicePitch = 0;

            Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateBuffer(
                    &vertexBufferDesc,
                    &vertexBufferData,
                    &vertexBuffer
                )
            );

            //add it to the mesh
            modelMesh->setVertexBuffer(vertexBuffer);
            modelMesh->setVertexCount(mesh->mNumVertices);
            modelMesh->setPrimitiveCount(mesh->mNumFaces);

            //get pointer to indices data
            std::vector<int> indices;
            for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
            {
                for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
                {
                    indices.push_back(mesh->mFaces[i].mIndices[j]);
                }
            }

            D3D11_BUFFER_DESC indexBufferDesc;
            indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
            indexBufferDesc.ByteWidth = (UINT)(sizeof(int) * indices.size());
            indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            indexBufferDesc.CPUAccessFlags = 0;
            indexBufferDesc.MiscFlags = 0;

            D3D11_SUBRESOURCE_DATA indexBufferData;
            indexBufferData.pSysMem = &indices[0];
            //indexBufferData.SysMemPitch = 0;
            //indexBufferData.SysMemSlicePitch = 0;

            Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateBuffer(
                    &indexBufferDesc,
                    &indexBufferData,
                    &indexBuffer
                )
            );

            //add it to the mesh
            modelMesh->setIndexBuffer(indexBuffer);
            modelMesh->setIndexCount((int)indices.size());


            if (createSRV)
            {
                D3D11_BUFFER_DESC bufferDesc;
                bufferDesc.Usage = D3D11_USAGE_DEFAULT;
                bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
                bufferDesc.CPUAccessFlags = 0;
                bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
                bufferDesc.ByteWidth = sizeof(VertexObject) * mesh->mNumVertices;
                bufferDesc.StructureByteStride = sizeof(VertexObject);

                Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

                DX::ThrowIfFailed(
                    m_deviceResources->GetD3DDevice()->CreateBuffer(&bufferDesc, &vertexBufferData, &buffer)
                );

                D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
                SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
                SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
                SRVDesc.Buffer.FirstElement = 0;
                SRVDesc.Buffer.NumElements = mesh->mNumVertices;

                Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;

                DX::ThrowIfFailed(
                    m_deviceResources->GetD3DDevice()->CreateShaderResourceView(buffer.Get(), &SRVDesc, &SRV)
                );

                modelMesh->setVertexSRV(SRV);


                //index Buffer SRV
                D3D11_BUFFER_DESC indexBufferDesc;
                indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
                indexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
                indexBufferDesc.CPUAccessFlags = 0;
                indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
                indexBufferDesc.ByteWidth = (UINT)(sizeof(int) * indices.size());
                indexBufferDesc.StructureByteStride = sizeof(int);

                Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

                DX::ThrowIfFailed(
                    m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer)
                );

                D3D11_SHADER_RESOURCE_VIEW_DESC indexSRVDesc;
                indexSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
                indexSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
                indexSRVDesc.Buffer.FirstElement = 0;
                indexSRVDesc.Buffer.NumElements = indices.size();

                Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> indexSRV;

                DX::ThrowIfFailed(
                    m_deviceResources->GetD3DDevice()->CreateShaderResourceView(indexBuffer.Get(), &indexSRVDesc, &indexSRV)
                );

                modelMesh->setindexSRV(indexSRV);

            }
        }

    }

    void ModelLoader::loadMeshBones(std::unique_ptr<Model>& model, const aiMesh* mesh)
    {
        for (int i = 0; i < mesh->mNumBones; ++i)
        {
            const aiBone* bone = mesh->mBones[i];

            int boneId = getBoneID(model, bone);

            if (boneId == model->m_boneInfo.size()) {
                Model::BoneInfo bi(FromMatrix(bone->mOffsetMatrix));
                // bi.OffsetMatrix.Print();
                model->m_boneInfo.push_back(bi);
            }

            for (int j = 0; j < bone->mNumWeights; j++) {
                const aiVertexWeight& vw = bone->mWeights[j];
                int globalVertexID = model->getMesh(model->getMeshCount() - 1)->getBaseVertex() + bone->mWeights[j].mVertexId;
                // printf("%d: %d %f\n",i, pBone->mWeights[i].mVertexId, vw.mWeight);
                model->m_Bones[globalVertexID].AddBoneData(boneId, vw.mWeight);
            }

            //MarkRequiredNodesForBone(pBone);
        }
    }

    int ModelLoader::getBoneID(std::unique_ptr<Model>& model, const aiBone* bone)
    {
        int boneIndex = 0;
        std::string boneName(bone->mName.C_Str());

        if (model->m_boneNameToIndexMap.find(boneName) == model->m_boneNameToIndexMap.end()) {
            // Allocate an index for a new bone
            boneIndex = (int)model->m_boneNameToIndexMap.size();
            model->m_boneNameToIndexMap[boneName] = boneIndex;
        }
        else {
            boneIndex = model->m_boneNameToIndexMap[boneName];
        }

        return boneIndex;
    }

    //determine the number of elements in the vertex
    int ModelLoader::GetNoofInputElements(aiMesh* mesh)
    {

        bool hasTexCoords = mesh->HasTextureCoords(0);
        bool hasColors = mesh->HasVertexColors(0);
        bool hasNormals = mesh->HasNormals();
        bool hasTangents = mesh->HasTangentsAndBitangents();
        bool hasBitangents = hasTangents;

        int noofElements = 1;

        if (hasColors)
            noofElements++;

        if (hasNormals)
            noofElements++;

        if (hasTangents)
            noofElements++;

        if (hasBitangents)
            noofElements++;

        if (hasTexCoords)
            noofElements++;

        return noofElements;
    }

    //some Assimp to SimpleMath conversion helpers
    Matrix ModelLoader::FromMatrix(aiMatrix4x4 mat)
    {
        Matrix m;
        m.m[0][0] = mat.a1;
        m.m[0][1] = mat.a2;
        m.m[0][2] = mat.a3;
        m.m[0][3] = mat.a4;
        m.m[1][0] = mat.b1;
        m.m[1][1] = mat.b2;
        m.m[1][2] = mat.b3;
        m.m[1][3] = mat.b4;
        m.m[2][0] = mat.c1;
        m.m[2][1] = mat.c2;
        m.m[2][2] = mat.c3;
        m.m[2][3] = mat.c4;
        m.m[3][0] = mat.d1;
        m.m[3][1] = mat.d2;
        m.m[3][2] = mat.d3;
        m.m[3][3] = mat.d4;
        return m;
    }

    Vector3 ModelLoader::FromVector(aiVector3D vec)
    {
        Vector3 v;
        v.x = vec.x;
        v.y = vec.y;
        v.z = vec.z;
        return v;
    }

    Color ModelLoader::FromColor(aiColor4D color)
    {
        Color c;
        c.R((byte)(color.r * 255));
        c.G((byte)(color.g * 255));
        c.B((byte)(color.b * 255));
        c.A((byte)(color.a * 255));
        return c;
    }
}
