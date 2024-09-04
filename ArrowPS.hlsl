cbuffer CBuf : register(b3)
{
    float4 ColorScale;
};

struct PSIn
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

float4 main(PSIn p) : SV_Target
{
    return float4(p.color.rgb, 0.1f);
}