cbuffer CBuf : register(b3)
{
    float4 color1;
};

struct PSIn
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

float4 main(PSIn p) : SV_Target
{
    return p.color;
}