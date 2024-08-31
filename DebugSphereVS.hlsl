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
VSOut main(float3 pos : Position,float3 color:color)
{
    VSOut VS;
    VS.pos = mul(float4(pos, 1.0f), modelViewProj);
    VS.color = float4(color, 1.0f);
    return VS;
}