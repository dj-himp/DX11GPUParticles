cbuffer initIndirectComputeArgs1DConstantBuffer : register(b4)
{
    uint nbThreadGroupX;

    uint3 padding;
}

RWBuffer<uint> indirectComputeArgs : register(u0);


[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    indirectComputeArgs[0] = (uint) ceil(indirectComputeArgs[0] / nbThreadGroupX);
}
