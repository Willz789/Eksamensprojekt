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
    float2 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 inst_pos : INST_POSITION;
    float inst_angle : INST_ANGLE;
    float inst_scale : INST_SCALE;
    float4 inst_col : INST_COLOR;
};

struct Output
{
    float2 tex : TEXCOORD;
    float4 col : COLOR;
    float4 svpos : SV_POSITION;
};

Output main(Input input)
{
    float cosAngle = cos(input.inst_angle), sinAngle = sin(input.inst_angle);

    float4 particlePos = mul(mul(float4(input.inst_pos, 1.0f), world), view);
    float2 billboard = input.pos * input.inst_scale;
    billboard = float2(billboard.x * cosAngle - billboard.y * sinAngle, billboard.x * sinAngle + billboard.y * cosAngle);
    
    float4 relativePos = float4(billboard, 0.0f, 0.0f);
    
    Output output;
    output.svpos = mul(particlePos + relativePos, projection);
    output.tex = input.tex;
    output.col = input.inst_col;
    
    return output;
}