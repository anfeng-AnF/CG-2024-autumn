cbuffer CBuf
{
    matrix modelViewProj;
    float r;
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
    float3 normal : NORMAL;
};

// 将输入线段扩展为矩形
// 输入4个顶点（两侧各一个邻接点）
[maxvertexcount(128)]
void main(lineadj VS_OUTPUT input[4], inout TriangleStream<GS_OUTPUT> OutputStream)
{
    float4 pPrev = input[0].pos;
    float4 p0 = input[1].pos;
    float4 p1 = input[2].pos;
    float4 pNext = input[3].pos;
    bool bIsBegin = false;
    if (all(pPrev == p0))
    {
        bIsBegin = true;
    }
    bool bIsEnd = false;
    if (all(p1 == pNext))
    {
        bIsEnd = true;
    }
    // 计算前后线段的方向向量
    float4 dirPrev = normalize(p0 - pPrev);
    float4 dirCur = normalize(p1 - p0);
    float4 dirNext = normalize(pNext - p1);

    // 计算法线方向
    float4 crossPrev = float4(normalize(cross(dirPrev.xyz, dirCur.xyz)), 0.0f);
    float4 crossNext = float4(normalize(cross(dirCur.xyz, dirNext.xyz)), 0.0f);
    
    float4 normalPrev = float4(normalize(cross(crossPrev.xyz, dirPrev.xyz)), 0.0f);
    float4 normalCurP = float4(normalize(cross(crossPrev.xyz, dirCur.xyz)), 0.0f);
    float4 normalCurN = float4(normalize(cross(crossNext.xyz, dirCur.xyz)), 0.0f);
    float4 normalNext = float4(normalize(cross(crossNext.xyz, dirNext.xyz)), 0.0f);
    
    float4 dirP = lerp(normalPrev, normalCurP, 0.5);
    float4 dirN = lerp(normalNext, normalCurN, 0.5);
    
    int n = 10;
    float4 vertexs0[10];
    float4 vertexs1[10];
    for (uint i = 0; i < n; i++)
    {
        float theta = 2.0f * 3.14159265359f * (float(i) / float(n));
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);
        vertexs0[i] = (p0 +r*0.5 * (cosTheta * crossPrev + sinTheta * dirP));
        vertexs1[i] = (p1 +r*0.5 * (cosTheta * crossNext + sinTheta * dirN));
    }
    GS_OUTPUT v;
    for (uint j = 0; j < n; j++)
    {
        v.pos = mul(vertexs0[(j + 1) % n], modelViewProj);
        v.normal = normalize(cross(vertexs1[j].xyz - vertexs0[j].xyz, vertexs0[(j + 1) % n].xyz - vertexs0[j].xyz));
        OutputStream.Append(v);

        v.pos = mul(vertexs0[j], modelViewProj);
        v.normal = normalize(cross(vertexs1[j].xyz - vertexs0[j].xyz, vertexs0[(j + 1) % n].xyz - vertexs0[j].xyz));
        OutputStream.Append(v);

        v.pos = mul(vertexs1[j], modelViewProj);
        v.normal = normalize(cross(vertexs1[j].xyz - vertexs0[j].xyz, vertexs0[(j + 1) % n].xyz - vertexs0[j].xyz));
        OutputStream.Append(v);
        OutputStream.RestartStrip();
    
        v.pos = mul(vertexs0[(j + 1) % n], modelViewProj);
        v.normal = normalize(-cross(vertexs1[(j + 1) % n].xyz - vertexs1[j].xyz, vertexs1[j].xyz - vertexs0[j].xyz));
        OutputStream.Append(v);

        v.pos = mul(vertexs1[j], modelViewProj);
        v.normal = normalize(-cross(vertexs1[(j + 1) % n].xyz - vertexs1[j].xyz, vertexs1[j].xyz - vertexs0[j].xyz));
        OutputStream.Append(v);

        v.pos = mul(vertexs1[(j + 1) % n], modelViewProj);
        v.normal = normalize(-cross(vertexs1[(j + 1) % n].xyz - vertexs1[j].xyz, vertexs1[j].xyz - vertexs0[j].xyz));
        OutputStream.Append(v);
        OutputStream.RestartStrip();
    
    // 封盖处理
        if (bIsBegin)
        {
            v.pos = mul(p0, modelViewProj);
            v.normal = normalize(cross(vertexs0[(j + 1) % n].xyz - vertexs0[j].xyz, p0.xyz - vertexs0[j].xyz));
            OutputStream.Append(v);

            v.pos = mul(vertexs0[j], modelViewProj);
            v.normal = normalize(cross(vertexs0[(j + 1) % n].xyz - vertexs0[j].xyz, p0.xyz - vertexs0[j].xyz));
            OutputStream.Append(v);

            v.pos = mul(vertexs0[(j + 1) % n], modelViewProj);
            v.normal = normalize(cross(vertexs0[(j + 1) % n].xyz - vertexs0[j].xyz, p0.xyz - vertexs0[j].xyz));
            OutputStream.Append(v);
            OutputStream.RestartStrip();
        }
    
        if (bIsEnd)
        {
            v.pos = mul(vertexs1[(j + 1) % n], modelViewProj);
            v.normal = normalize(cross(vertexs1[(j + 1) % n].xyz - vertexs1[j].xyz, p1.xyz - vertexs1[j].xyz));
            OutputStream.Append(v);

            v.pos = mul(vertexs1[j], modelViewProj);
            v.normal = normalize(cross(vertexs1[(j + 1) % n].xyz - vertexs1[j].xyz, p1.xyz - vertexs1[j].xyz));
            OutputStream.Append(v);

            v.pos = mul(p1, modelViewProj);
            v.normal = normalize(cross(vertexs1[(j + 1) % n].xyz - vertexs1[j].xyz, p1.xyz - vertexs1[j].xyz));
            OutputStream.Append(v);
            OutputStream.RestartStrip();
        }
    }
}

