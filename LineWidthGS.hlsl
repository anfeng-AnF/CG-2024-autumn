cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};
// 输入的线段顶点
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

// 输出的四边形顶点
struct GS_OUTPUT
{
    float4 pos : SV_POSITION;
};

// 将输入线段扩展为矩形
// 输入4个顶点（两侧各一个邻接点）
[maxvertexcount(128)]
void main(lineadj VS_OUTPUT input[4], inout TriangleStream<GS_OUTPUT> OutputStream)
{
    float4 pPrev = input[0].pos; // 前一条线段的第一个顶点
    float4 p0 = input[1].pos; // 当前线段的第一个顶点
    float4 p1 = input[2].pos; // 当前线段的第二个顶点
    float4 pNext = input[3].pos; // 后一条线段的第二个顶点

    // 计算前后线段的方向向量
    float4 dirPrev = normalize(p0 - pPrev);
    float4 dirCur = normalize(p1 - p0);
    float4 dirNext = normalize(pNext - p1);

    // 计算法线方向
    float4 normalPrev = float4(-dirPrev.y, dirPrev.x, 0.0f, 0.0f);
    float4 normalCur = float4(-dirCur.y, dirCur.x, 0.0f, 0.0f);
    float4 normalNext = float4(-dirNext.y, dirNext.x, 0.0f, 0.0f);

    // 插值当前线段的法线
    float4 normalStart = normalize(lerp(normalPrev, normalCur, 0.5));
    float4 normalEnd = normalize(lerp(normalCur, normalNext, 0.5));
    
    float r = 0.05f;
    
    GS_OUTPUT v0;
    GS_OUTPUT v1;
    GS_OUTPUT v2;
    GS_OUTPUT v3;
    
    v0.pos  = p0 - normalStart * r * 0.5;
    v1.pos  = p0 + normalStart * r * 0.5;
    v2.pos  = p1 + normalEnd * r * 0.5;
    v3.pos  = p1 - normalEnd * r * 0.5;
    
    // 输出两个三角形
    OutputStream.Append(v0);
    OutputStream.Append(v1);
    OutputStream.Append(v2);

    // 输出第二个三角形
    OutputStream.RestartStrip();
    OutputStream.Append(v2);
    OutputStream.Append(v3);
    OutputStream.Append(v0);
}

