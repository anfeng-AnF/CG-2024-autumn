cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float4 pos : SV_Position;
    float4 color : Color;
};

VSOut main(float3 pos : Position, float4 color : Color)
{
    VSOut vso;
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.color = color;
    return vso;
}