
struct Input
{
    float2 tex : TEXCOORD;
    float4 col : COLOR;
};

float circle(float2 uv)
{
    uv = uv * 2.0f - 1.0f; // map to [-1, 1]
    return step(dot(uv, uv), 1.0f);
}

float4 main(Input input) : SV_TARGET
{
    return float4(input.col.xyz, input.col.a * circle(input.tex));
}