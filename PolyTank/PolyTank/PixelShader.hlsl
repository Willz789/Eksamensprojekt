
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

Texture2D shadowMap : register(t0);
SamplerState shadowSampler : register(s0);

struct Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 shadowpos : SHADOW_POSITION;
};

static const float pi = 3.14159265359f;
static const float inv_pi = 1.0f / pi;

static const float shadowBias = 0.0009f;

float visible(float4 shadowpos)
{
    float3 proj = shadowpos.xyz / shadowpos.w;
    proj.y *= -1.0f;
    proj.xy = proj.xy * 0.5f + 0.5f; // map from clipspace [-1, 1] to uvspace [0, 1]
    
    float shadowDepth = shadowMap.Sample(shadowSampler, proj.xy);
    
    return step(proj.z, shadowDepth + shadowBias);    
}

float4 main(Input input) : SV_TARGET
{
    float3 reflectance = base_color * inv_pi;
    
    float3 n = normalize(input.normal);
    float3 i = -sun.direction;
    
    float3 color = reflectance * ambient.color; 
    color += reflectance * max(0.0f, dot(n, i)) * sun.color * visible(input.shadowpos);
    
    return float4(color, 1.0f);
}