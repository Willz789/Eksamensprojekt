
struct Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
};

float4 main(Input input) : SV_TARGET
{
    return float4(input.normal, 1.0f);
}