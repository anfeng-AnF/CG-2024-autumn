cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSout
{
    float4 pos : SV_Position;
    float4 color : Color;
};

VSout main(float3 pos : Position,float4 color:Color)
{
    VSout vsOut;
    vsOut.pos = mul(float4(pos, 1.0f), modelViewProj);
    vsOut.color = color;
    return vsOut;
}