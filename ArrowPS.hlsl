cbuffer CBuf : register(b3)
{
    float4 color1;
};

float4 main(float4 color:Color4) : SV_Target
{
    return color;
}