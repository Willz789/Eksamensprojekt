
cbuffer PerFrame : register(b0) {
    float4x4 projection;
    float4x4 view;
};

cbuffer PerObject : register(b1) {
    float4x4 world;
    float4x4 world_normals;
};

struct Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct Output
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 svpos : SV_POSITION;
};

Output main(Input input) 
{    
    Output output;
    output.pos = mul(mul(float4(input.pos, 1.0f), world), view).xyz;
    output.normal = mul(mul(float4(input.normal, 0.0f), world_normals), view).xyz;
    output.texcoord = input.texcoord;
    
    output.svpos = mul(float4(output.pos, 1.0f), projection);
    
    return output;
}
