cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};
struct VSOut
{
    float4 pos : SV_Position;
};
VSOut main(float3 pos : Position)
{
    VSOut VS;
    VS.pos = mul(float4(pos, 1.0f), modelViewProj);
    return VS;
}