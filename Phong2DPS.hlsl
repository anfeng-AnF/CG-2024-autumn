cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;


float4 main(float3 viewPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    float3 lightDir = normalize(float3(0.0, 1.0, -1.0));
    float3 norm = normalize(n);

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
        transitionFactor = smoothstep(colorEdge0, colorEdge1, dotVal) * 0.5 + 0.3; // �����߽�˳���Է������
    }

    // ���������ɫ
    float4 transitionColor = float4(0.8, 0.6, 0.5, 1.0) * 0.5;

    // �������
    float4 texColor = tex.Sample(splr, tc);

    // ���������ɫ
    float4 baseColor = diff * texColor;

    // �ڹ��������ڻ�ϻ�����ɫ�͹�����ɫ
    float4 finalColor = lerp(baseColor, transitionColor, transitionFactor);

    return finalColor;
}