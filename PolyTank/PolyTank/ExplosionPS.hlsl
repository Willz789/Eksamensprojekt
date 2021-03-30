
struct Input
{
    float2 tex : TEXCOORD;
    float4 col : COLOR;
};

float4 main(Input input) : SV_TARGET
{
    return input.col;
}