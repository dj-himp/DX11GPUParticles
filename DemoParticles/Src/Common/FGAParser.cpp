#include "pch.h"
#include "FGAParser.h"

#include <sstream>
#include <iterator>
#include <filesystem>

using namespace DirectX::SimpleMath;

namespace DemoParticles
{

    void FGAParser::parse(const char* filename, FGAContent& content)
    {
        std::filesystem::path filePath(filename);
        if (filePath.extension().compare(".fga") == 0)
        {
            parseFGA(filename, content);
        }
        else if (filePath.extension().compare(".vf") == 0)
        {
            parseVF(filename, content);
        }
    }

    void FGAParser::parseFGA(const char* filename, FGAContent& content)
    {
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
            content.forces[i] = Vector4(force.x, force.y, force.z, -1.0f);
            i++;
        }

        file.close();
    }

    void FGAParser::parseVF(const char* filename, FGAContent& content)
    {
        std::ifstream file;
        file.open(filename, std::ios::binary);

        assert(file);

        //VF_F == SDF (list of distances)
        //VF_V == Gradient (list of gradients)
        char header[4];
        for (int i = 0; i < 4; ++i)
        {
            file.read(&header[i], 1);
        }

        UINT8 size[3];
        for (int i = 0; i < 3; ++i)
        {
            file.read(reinterpret_cast<char*>(&size[i]), 1);
            char trash;
            file.read(&trash, 1);
        }

        content.sizeX = size[0];
        content.sizeY = size[1];
        content.sizeZ = size[2];

        content.forces.resize(content.sizeX * content.sizeY * content.sizeZ);

        for (int i = 0; i < content.forces.size(); ++i)
        {
            file.read(reinterpret_cast<char*>(&content.forces[i].x), sizeof(float));
            file.read(reinterpret_cast<char*>(&content.forces[i].y), sizeof(float));
            file.read(reinterpret_cast<char*>(&content.forces[i].z), sizeof(float));
            content.forces[i].w = -1.0f;
        }

        content.boundMin = Vector3(-800.0f, -800.0f, -800.0f);
        content.boundMax = Vector3(800.0f, 800.0f, 800.0f);
        
        file.close();

        //load SDF if present
        {
            std::string SDFFilename(filename);
            SDFFilename += "d";
            std::ifstream file;
            file.open(SDFFilename, std::ios::binary);

            if (!file)
            {
                DebugUtils::log("SDF file not found");
                return;
            }

            //VF_F == SDF (list of distances)
            //VF_V == Gradient (list of gradients)
            char header[4];
            for (int i = 0; i < 4; ++i)
            {
                file.read(&header[i], 1);
            }

            UINT8 size[3];
            for (int i = 0; i < 3; ++i)
            {
                file.read(reinterpret_cast<char*>(&size[i]), 1);
                char trash;
                file.read(&trash, 1);
            }

            for (int i = 0; i < content.forces.size(); ++i)
            {
                file.read(reinterpret_cast<char*>(&content.forces[i].w), sizeof(float));
            }
        }
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
