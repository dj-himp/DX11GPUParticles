#pragma once

#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <assimp/color4.h>

namespace AssimpToDX
{
    inline DirectX::SimpleMath::Matrix FromMatrix(aiMatrix4x4 m)
    {
        /*DirectX::SimpleMath::Matrix m;
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
        return m;*/

        return DirectX::SimpleMath::Matrix
		    (m.a1, m.a2, m.a3, m.a4,
			m.b1, m.b2, m.b3, m.b4,
			m.c1, m.c2, m.c3, m.c4,
			m.d1, m.d2, m.d3, m.d4);
    }

    inline DirectX::SimpleMath::Vector3 FromVector(aiVector3D vec)
    {
        DirectX::SimpleMath::Vector3 v;
        v.x = vec.x;
        v.y = vec.y;
        v.z = vec.z;
        return v;
    }

    inline DirectX::SimpleMath::Color FromColor(aiColor4D color)
    {
        DirectX::SimpleMath::Color c;
        c.R((byte)(color.r * 255));
        c.G((byte)(color.g * 255));
        c.B((byte)(color.b * 255));
        c.A((byte)(color.a * 255));
        return c;
    }
}
