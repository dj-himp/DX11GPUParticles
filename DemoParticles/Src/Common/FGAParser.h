#pragma once

namespace DemoParticles
{
    class FGAParser
    {
    public:
        FGAParser() {}
        //~FGAParser();

        Microsoft::WRL::ComPtr<ID3D11Texture3D> parse(const char* filename, const DX::DeviceResources* deviceResources);

    private:

        struct FGAContent
        {
            int sizeX;
            int sizeY;
            int sizeZ;
            DirectX::SimpleMath::Vector3 boundMin;
            DirectX::SimpleMath::Vector3 boundMax;
            std::vector<DirectX::SimpleMath::Vector4> forces;
        };

        char* parseUntilDelimiter(char* buffer, char delimiter);
        char* parseFloat(char* buffer, float& outFloat);
        char* parseInt(char* buffer, int& outInt);
        char* parseVector3(char* buffer, DirectX::SimpleMath::Vector3& outInt);
    };
}
