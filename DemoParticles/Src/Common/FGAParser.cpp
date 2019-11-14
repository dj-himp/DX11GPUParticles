#include "pch.h"
#include "FGAParser.h"

#include <sstream>

using namespace DirectX::SimpleMath;

namespace DemoParticles
{

    Microsoft::WRL::ComPtr<ID3D11Texture3D> FGAParser::parse(const char* filename, const DX::DeviceResources* deviceResources)
    {
        std::ifstream file;
        file.open(filename, std::ios::in);
        
        assert(file);

        std::stringstream strStream;
        strStream << file.rdbuf();
        const std::string contentStr = strStream.str();

        FGAContent content;

        char* currentParse = (char*)contentStr.c_str();
        currentParse = parseInt(currentParse, content.sizeX);
        currentParse = parseInt(currentParse, content.sizeY);
        currentParse = parseInt(currentParse, content.sizeZ);

        currentParse = parseVector3(currentParse, content.boundMin);
        currentParse = parseVector3(currentParse, content.boundMax);

        content.forces.resize(content.sizeX * content.sizeY * content.sizeZ);

        int i = 0;
        while (*currentParse != 0)
        {
            Vector3 force;
            currentParse = parseVector3(currentParse, force);
            content.forces[i] = Vector4(force.x, force.y, force.z, 0.0);
            i++;
        }
                
        file.close();

        D3D11_TEXTURE3D_DESC desc;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.Width = content.sizeX;
        desc.Height = content.sizeY;
        desc.Depth = content.sizeZ;
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.MipLevels = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &content.forces[0];
        data.SysMemPitch = content.sizeX;
        data.SysMemSlicePitch = content.sizeY;

        Microsoft::WRL::ComPtr<ID3D11Texture3D> texture;

        DX::ThrowIfFailed(
            deviceResources->GetD3DDevice()->CreateTexture3D(&desc, &data, &texture)
        );

        return texture;
    }

    char* FGAParser::parseFloat(char* buffer, float& outFloat)
    {
        outFloat = std::atof(buffer);
        return parseUntilDelimiter(buffer, ',');
    }

    char* FGAParser::parseInt(char* buffer, int& outInt)
    {
        outInt = std::atoi(buffer);
        return parseUntilDelimiter(buffer, ',');
    }

    char* FGAParser::parseVector3(char* buffer, Vector3& outVec3)
    {
        buffer = parseFloat(buffer, outVec3.x);
        buffer = parseFloat(buffer, outVec3.y);
        buffer = parseFloat(buffer, outVec3.z);
        
        return buffer;
    }

    char* FGAParser::parseUntilDelimiter(char* buffer, char delimiter)
    {
        assert(buffer);

        while (*buffer != 0 && *buffer != delimiter)
        {
            buffer++;
        }
        if (*buffer == delimiter)
        {
            buffer++;
        }

        return buffer;
    }
}
