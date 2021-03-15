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
    output.pos = float4(input.pos, 1.0f);
    output.color = input.color;
    
    return output;
}
