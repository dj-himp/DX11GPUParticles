static uint rng_state;

//use this to hash the seed if more randomness is needed
uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

//fastest
uint rand_lcg()
{
    // LCG values from Numerical Recipes
    rng_state = 1664525 * rng_state + 1013904223;
    return rng_state;
}

//better
uint rand_xorshift()
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

//random float number between 0-1
float rand_xorshift_normalized()
{
    return float(rand_xorshift()) * (1.0 / 4294967296.0);
}

//Usage example
/*
[numthreads(256, 1, 1)]
void cs_main(uint3 threadId : SV_DispatchThreadID)
{
    // Seed the PRNG using the thread ID
    rng_state = threadId.x;

    // Generate a few numbers...
    uint r0 = rand_xorshift();
    uint r1 = rand_xorshift();
    // Do some stuff with them...

    // Generate a random float in [0, 1)...
    float f0 = float(rand_xorshift()) * (1.0 / 4294967296.0);

    // ...etc.
}
*/