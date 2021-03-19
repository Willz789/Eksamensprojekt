
cbuffer PerFrame : register(b0) {
    float4x4 cameraProjection;
};

cbuffer PerObject : register(b1) {
    float4x4 transform;
};

struct Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
};

struct Output
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 svpos : SV_POSITION;
};

Output main(Input input) 
{    
    Output output;
    output.pos = mul(float4(input.pos, 1.0f), transform).xyz;
    
    output.normal = normalize(
        mul(float4(input.normal, 0.0f), transform).xyz
    );
    
    output.tangent = float4(normalize(
        mul(float4(input.tangent.xyz, 0.0f), transform).xyz
    ), input.tangent.w);
    
    output.texcoord = input.texcoord;
    
    output.svpos = mul(float4(output.pos, 1.0f), cameraProjection);
    
    return output;
}
