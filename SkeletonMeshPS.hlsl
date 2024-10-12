// ���������ṹ��
struct PSIn
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position; // �����Ҫ�ں���������ʹ��
};

// ���������
Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

float4 main(PSIn input) : SV_Target
{
    float3 lightDir = normalize(float3(0.0, 1.0, -1.0));
    float3 norm = normalize(input.normal);

    // ������������շ���
    float dotVal = dot(norm, lightDir);

    // ������Ӱ���ɴ��߽�
    float shadowEdge0 = 0.4;
    float shadowEdge1 = 0.5;
    float diff = smoothstep(shadowEdge0, shadowEdge1, dotVal) * 0.4 + 0.4; // ʹ diff �� 0.4 �� 0.8 ƽ������

    // ������ɫ���ɱ߽�����
    float colorEdge0 = 0.5;
    float colorEdge1 = 0.4;
    float transitionFactor = 0.0;

    // ��� dotVal ����ɫ���������ڣ�������������
    if (dotVal > colorEdge1 && dotVal < colorEdge0)
    {
        transitionFactor = smoothstep(colorEdge0, colorEdge1, dotVal)*0.5+0.3; // �����߽�˳���Է������
    }

    // ���������ɫ
    float4 transitionColor = float4(0.8, 0.6, 0.5, 1.0)*0.5;

    // �������
    float4 texColor = textureMap.Sample(samplerState, input.tc);

    // ���������ɫ
    float4 baseColor = diff * texColor;

    // �ڹ��������ڻ�ϻ�����ɫ�͹�����ɫ
    float4 finalColor = lerp(baseColor, transitionColor, transitionFactor);

    return finalColor;
}




