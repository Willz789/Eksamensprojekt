
cbuffer PerPass : register(b0)
{
    float4x4 projection;
    float4x4 view;
}

cbuffer PerObject : register(b1)
{
    float4x4 world;
}


float4 main(float3 pos : POSITION) : SV_POSITION
{
    float4 outpos = float4(pos, 1.0f);
    outpos = mul(outpos, world);
    outpos = mul(outpos, view);
    outpos = mul(outpos, projection);
    
    return outpos;

}