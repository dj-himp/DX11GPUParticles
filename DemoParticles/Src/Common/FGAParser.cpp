#include "pch.h"
#include "FGAParser.h"

#include <sstream>
#include <iterator>

using namespace DirectX::SimpleMath;

namespace DemoParticles
{

    void FGAParser::parse(const char* filename, FGAContent& content)
    {
        /*std::ifstream file;
        file.open(filename, std::ios::binary);

        assert(file);

        float x, y, z, w;
        file.read(reinterpret_cast<char*>(&x), sizeof(float));
        file.read(reinterpret_cast<char*>(&y), sizeof(float));
        file.read(reinterpret_cast<char*>(&z), sizeof(float));
        file.read(reinterpret_cast<char*>(&w), sizeof(float));*/

        //std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});


        std::ifstream file;
        file.open(filename, std::ios::in);
        
        assert(file);

        std::stringstream strStream;
        strStream << file.rdbuf();
        const std::string contentStr = strStream.str();

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
            content.forces[i] = Vector4(force.x, force.y, force.z, 0.0f);
            i++;
        }
                
        file.close();

    }

    char* FGAParser::parseFloat(char* buffer, float& outFloat)
    {
        outFloat = (float)std::atof(buffer);
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
