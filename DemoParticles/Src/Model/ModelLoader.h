#pragma once

#include <assimp/Importer.hpp>

struct aiScene;
struct aiNode;
struct aiMesh;

namespace DemoParticles
{    
    class Model;

    class ModelLoader
    {
    public:

        ModelLoader(const DX::DeviceResources* deviceResources);
        
        std::unique_ptr<Model> load(const std::string fileName, const bool createSRV = false);

    private:

        void LoadMeshes(std::unique_ptr<Model>& model, const aiScene* scene, const bool createSRV);
        void InitMesh(std::unique_ptr<Model>& model, const aiScene* scene, const bool createSRV);
        int GetNoofInputElements(aiMesh* mesh);

        const DX::DeviceResources* m_deviceResources;

        Assimp::Importer m_importer;
    };
}