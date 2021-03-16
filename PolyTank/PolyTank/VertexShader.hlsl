cbuffer Transform : register(b0) {
    float4x4 transform;
    float4x4 projection;
};

struct Input
{
    float3 pos : POSITION;
    float3 color : COLOR;
};

struct Output
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

Output main(Input input) 
{    
    Output output;
    output.pos = mul(float4(input.pos, 1.0f), transform);
    output.pos = mul(output.pos, projection);
    output.color = input.color;
    
    return output;
}
