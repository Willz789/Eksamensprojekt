
struct Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

float4 main(Input input) : SV_TARGET
{
    float3 n = normalize(input.normal);
    
    return float4(n, 1.0f);
}