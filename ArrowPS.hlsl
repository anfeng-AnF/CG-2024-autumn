cbuffer CBuf
{
    float scale;
};

float4 main(float4 color:Color) : SV_Target
{
    return float4(color.rgb * scale, color.a);
}