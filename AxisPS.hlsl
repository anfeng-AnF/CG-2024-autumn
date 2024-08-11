struct PSIn
{
    float4 pos : SV_Position; // 顶点位置，顶点着色器输出的位置
    float4 color : COLOR; // 顶点颜色，顶点着色器输出的颜色
};

float4 main(PSIn p) : SV_Target
{
    return p.color; // 返回颜色作为最终的像素输出
}
