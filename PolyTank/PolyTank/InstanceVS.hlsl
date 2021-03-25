
cbuffer PerFrame : register(b0)
{
    float4x4 projection;
    float4x4 view;
    float4x4 shadow_projection;
    float4x4 shadow_view;
};

cbuffer PerObject : register(b1)
{
    float4x4 world;
    float4x4 world_normals;
};

struct Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 inst_pos : INST_POSITION;
};

struct Output
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 shadowpos : SHADOW_POSITION;
    float4 svpos : SV_POSITION;
};

Output main(Input input)
{
    Output output;
    float4 worldPos = mul(float4(input.pos + input.inst_pos, 1.0f), world);
    
    output.pos = mul(worldPos, view).xyz;
    output.normal = mul(mul(float4(input.normal, 0.0f), world_normals), view).xyz;
    output.texcoord = input.texcoord;
    output.shadowpos = mul(mul(worldPos, shadow_view), shadow_projection);
    
    output.svpos = mul(float4(output.pos, 1.0f), projection);
    
    return output;
}