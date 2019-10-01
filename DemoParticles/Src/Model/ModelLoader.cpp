#include "pch.h"
#include "ModelLoader.h"

#include "Model.h"
#include "ModelMesh.h"
#include "..\Content\ShaderStructures.h"
#include "..\Common\DirectXHelper.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ModelLoader::ModelLoader(const std::shared_ptr<DX::DeviceResources>& deviceResources)
        : m_deviceResources(deviceResources)
    {

    }

    std::unique_ptr<DemoParticles::Model> ModelLoader::load(const std::string fileName)
    {
        std::unique_ptr<Model> model = std::make_unique<Model>(m_deviceResources);

        //doc said to convert to left handed in the importer if using directX (need to investigate)
        //answer : DirectX convention is left handed but as DirectTK is right handed living it like that
        const aiScene* scene = m_importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality /*aiProcessPreset_TargetRealtime_Fast*/);

        if(!scene)
        {
            std::cerr << "[ModelLoader] Failed to load " << fileName << " with error : " << m_importer.GetErrorString() << "\n";
            assert(0);
        }

        Matrix transform = Matrix::Identity;
        AddVertexData(model, scene, scene->mRootNode, transform);


        return std::move(model);
    }

    //Create meshes and add vertex and index buffers
    void ModelLoader::AddVertexData(std::unique_ptr<Model>& model, const aiScene* scene, const aiNode* node, Matrix& transform)
    {
        Matrix previousTransform = transform;
        transform = previousTransform * FromMatrix(node->mTransformation);

        //also calculate inverse transpose matrix for normal/tangent/bitagent transformation
        Matrix invTranspose = transform;
        invTranspose.Invert();
        invTranspose.Transpose();

        
        for (unsigned int meshId = 0; meshId < node->mNumMeshes; ++meshId)
        {
            //get a mesh from the scene
            aiMesh* mesh = scene->mMeshes[meshId];

            //create new mesh to add to model
            std::unique_ptr<ModelMesh>& modelMesh = model->AddMesh();

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

            //set the vertex elements and size
            modelMesh->setInputElements(vertexElements);
            modelMesh->setVertexSize(vertexSize);

            //get pointers to vertex data
            aiVector3D* positions = mesh->mVertices;
            aiVector3D* texCoords = mesh->mTextureCoords[0];
            aiVector3D* normals = mesh->mNormals;
            aiVector3D* tangents = mesh->mTangents;
            aiVector3D* biTangents = mesh->mBitangents;
            aiColor4D* colours = mesh->mColors[0];

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

            //create data stream for vertices
            std::vector<VertexObject> vertices(mesh->mNumVertices);
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                //add position, after transforming it with accumulated node transform
                {
                    Vector3 pos = FromVector(positions[i]);
                    Vector4 result = Vector3::Transform(pos, transform);
                    vertices[i].position = Vector3(result.x, result.y, result.z);
                    //vertices[i].position = pos;
                }

                if (hasColors)
                {
                    vertices[i].color = FromColor(mesh->mColors[0][i]);
                }
                if (hasNormals)
                {
                    Vector3 normal = FromVector(normals[i]);
                    
                    Vector4 result = Vector3::Transform(normal, invTranspose);
                    vertices[i].normal = Vector3(result.x, result.y, result.z);
                }
                if (hasTangents)
                {
                    Vector3 tangent = FromVector(tangents[i]);
                    Vector4 result = Vector3::Transform(tangent, invTranspose);
                    vertices[i].tangent = Vector3(result.x, result.y, result.z);
                }
                if (hasBitangents)
                {
                    Vector3 biTangent = FromVector(biTangents[i]);
                    Vector4 result = Vector3::Transform(biTangent, invTranspose);
                    vertices[i].bitangent = Vector3(result.x, result.y, result.z);
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
        }

        //if node has more children process them as well
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            AddVertexData(model, scene, node->mChildren[i], transform);
        }

        transform = previousTransform;
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
