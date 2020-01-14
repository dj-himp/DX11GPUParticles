#pragma once
/*
Class to parse FGA file (unreal vector field format) &&
parse VF file (unity format)

For VF file I renamed extentions as follow (but the format remain the same)
.vf => vector field (Distance Gradient) (float3)
.vfd => Signed distance field (float)

*/


namespace DemoParticles
{
    class FGAParser
    {
    public:

        struct FGAContent
        {
            int sizeX;
            int sizeY;
            int sizeZ;
            DirectX::SimpleMath::Vector3 boundMin;
            DirectX::SimpleMath::Vector3 boundMax;
            std::vector<DirectX::SimpleMath::Vector4> forces; //xyz contains gradient & w contain SDF (if available)
        };

        FGAParser() {}
        //~FGAParser();

         void parse(const char* filename, FGAContent& content);

    private:

        void parseFGA(const char* filename, FGAContent& content);
        void parseVF(const char* filename, FGAContent& content);

        char* parseUntilDelimiter(char* buffer, char delimiter);
        char* parseFloat(char* buffer, float& outFloat);
        char* parseInt(char* buffer, int& outInt);
        char* parseVector3(char* buffer, DirectX::SimpleMath::Vector3& outInt);
    };
}
