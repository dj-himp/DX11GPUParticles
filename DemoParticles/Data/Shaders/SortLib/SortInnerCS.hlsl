//
// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#if( SORT_SIZE>2048 )
	#error
#endif

#define NUM_THREADS		(SORT_SIZE/2)
#define INVERSION		(16*2 + 8*3)

//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
cbuffer NumElementsCB : register( b1 )
{
	int4 g_NumElements;
};

//--------------------------------------------------------------------------------------
// Structured Buffers
//--------------------------------------------------------------------------------------
RWStructuredBuffer<float2> Data : register( u0 );


//--------------------------------------------------------------------------------------
// Bitonic Sort Compute Shader
//--------------------------------------------------------------------------------------
groupshared float2	g_LDS[SORT_SIZE];


[numthreads(NUM_THREADS, 1, 1)]
void BitonicInnerSort(	uint3 Gid	: SV_GroupID, 
						uint3 DTid	: SV_DispatchThreadID, 
						uint3 GTid	: SV_GroupThreadID, 
						uint	GI	: SV_GroupIndex )
{
	int4 tgp;

	tgp.x = Gid.x * 256;
	tgp.y = 0;
	tgp.z = g_NumElements.x;
	tgp.w = min( 512, max( 0, g_NumElements.x - Gid.x * 512 ) );

	int GlobalBaseIndex = tgp.y + tgp.x*2 + GTid.x;
	int LocalBaseIndex  = GI;
	int i;

    // Load shared data
	[unroll]for( i = 0; i<2; ++i )
	{
		if( GI+i*NUM_THREADS<tgp.w )
			g_LDS[ LocalBaseIndex + i*NUM_THREADS ] = Data[ GlobalBaseIndex + i*NUM_THREADS ];
	}
    GroupMemoryBarrierWithGroupSync();

	// sort threadgroup shared memory
	for( int nMergeSubSize=SORT_SIZE>>1; nMergeSubSize>0; nMergeSubSize=nMergeSubSize>>1 ) 
	{			
		int tmp_index = GI;
		int index_low = tmp_index & (nMergeSubSize-1);
		int index_high = 2*(tmp_index-index_low);
		int index = index_high + index_low;

		unsigned int nSwapElem = index_high + nMergeSubSize + index_low;

		if( nSwapElem<tgp.w )
		{
			float2 a = g_LDS[index];
			float2 b = g_LDS[nSwapElem];

			if (a.x > b.x)
			{ 
				g_LDS[index] = b;
				g_LDS[nSwapElem] = a;
			}
		}
		GroupMemoryBarrierWithGroupSync();
	}
    
    // Store shared data
	[unroll]for( i = 0; i<2; ++i )
	{
		if( GI+i*NUM_THREADS<tgp.w )
			Data[ GlobalBaseIndex + i*NUM_THREADS ] = g_LDS[ LocalBaseIndex + i*NUM_THREADS ];
	}
}
