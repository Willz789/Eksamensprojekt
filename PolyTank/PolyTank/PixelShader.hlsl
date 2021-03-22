
cbuffer Lighting : register(b0)
{
    struct
    {
        float3 direction;
        float3 color;
    } sun;
    
    struct
    {
        float3 color;
    } ambient;
};

cbuffer Material : register(b1)
{
    float3 base_color;
}

struct Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

static const float pi = 3.14159265359f;
static const float inv_pi = 1.0f / pi;

float4 main(Input input) : SV_TARGET
{
    float3 reflectance = base_color * inv_pi;
    
    float3 n = normalize(input.normal);
    float3 i = -sun.direction;
    
    float3 color = reflectance * ambient.color; 
    color += reflectance * max(0.0f, dot(n, i)) * sun.color;
    
    return float4(color, 1.0f);
}