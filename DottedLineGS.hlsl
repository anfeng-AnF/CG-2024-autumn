struct GSOutput
{
    float4 pos : SV_POSITION;
    float4 color : Color; // 从顶点着色器传递的颜色
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : Color; // 从顶点着色器传递的颜色
};

// 定义虚线段长度和间隔
#define DASH_LENGTH 0.1
#define GAP_LENGTH 0.05

[maxvertexcount(64)]
void main(
    line VSOutput input[2],
    inout LineStream<GSOutput> output
)
{
    float4 start = input[0].pos;
    float4 end = input[1].pos;
    float4 color = input[0].color; // 从顶点着色器传递的颜色

    // 计算线段的长度和方向
    float3 direction = end.xyz - start.xyz;
    float lengthd = length(direction);
    direction /= lengthd;
    
    // 计算虚线段的总长度
    float dashGapLength = DASH_LENGTH + GAP_LENGTH;
    int numDashes = (int) (lengthd / dashGapLength);

    // 生成虚线段
    float currentDistance = 0.0;
    for (int i = 0; i < numDashes; ++i)
    {
        // 计算当前虚线段的起点和终点
        float segmentStart = currentDistance;
        float segmentEnd = segmentStart + DASH_LENGTH;
        if (segmentEnd > lengthd)
        {
            segmentEnd = lengthd; // 修剪最后一段
        }

        // 输出虚线段的起点
        GSOutput outputStart;
        outputStart.pos = start + float4(direction * segmentStart, 0.0f);
        outputStart.color = color;
        output.Append(outputStart);

        // 输出虚线段的终点
        GSOutput outputEnd;
        outputEnd.pos = start + float4(direction * segmentEnd, 0.0f);
        outputEnd.color = color;
        output.Append(outputEnd);

        currentDistance += dashGapLength;
    }

    // 确保输出流结束
    output.RestartStrip();
}
