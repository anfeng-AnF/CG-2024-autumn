struct GSOutput
{
    float4 pos : SV_POSITION;
    float4 color : Color; // �Ӷ�����ɫ�����ݵ���ɫ
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : Color; // �Ӷ�����ɫ�����ݵ���ɫ
};

// �������߶γ��Ⱥͼ��
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
    float4 color = input[0].color; // �Ӷ�����ɫ�����ݵ���ɫ

    // �����߶εĳ��Ⱥͷ���
    float3 direction = end.xyz - start.xyz;
    float lengthd = length(direction);
    direction /= lengthd;
    
    // �������߶ε��ܳ���
    float dashGapLength = DASH_LENGTH + GAP_LENGTH;
    int numDashes = (int) (lengthd / dashGapLength);

    // �������߶�
    float currentDistance = 0.0;
    for (int i = 0; i < numDashes; ++i)
    {
        // ���㵱ǰ���߶ε������յ�
        float segmentStart = currentDistance;
        float segmentEnd = segmentStart + DASH_LENGTH;
        if (segmentEnd > lengthd)
        {
            segmentEnd = lengthd; // �޼����һ��
        }

        // ������߶ε����
        GSOutput outputStart;
        outputStart.pos = start + float4(direction * segmentStart, 0.0f);
        outputStart.color = color;
        output.Append(outputStart);

        // ������߶ε��յ�
        GSOutput outputEnd;
        outputEnd.pos = start + float4(direction * segmentEnd, 0.0f);
        outputEnd.color = color;
        output.Append(outputEnd);

        currentDistance += dashGapLength;
    }

    // ȷ�����������
    output.RestartStrip();
}
