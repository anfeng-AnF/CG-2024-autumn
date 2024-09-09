cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};
VS_OUTPUT main(float3 pos : Position)
{
    VS_OUTPUT vsout;
    vsout.pos = float4(pos, 1.0f);
    vsout.normal = float3(0.0f, 0.0f, 1.0f);
    return vsout;
}