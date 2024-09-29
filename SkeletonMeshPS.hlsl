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

// ������ɫ��
float4 main(PSIn input) : SV_Target
{
    // ����򵥵����������
    float3 lightDir = normalize(float3(0.0, 1.0, -1.0)); // �����Դ��(0, 1, -1)λ��
    float3 norm = normalize(input.normal); // ���߹�һ��

    // ���������
    float diff;
    float dotVal = dot(norm, lightDir);
    if (dotVal> 0.4)
    {
        diff = 0.8;
    }
    else
    {
        diff = 0.4;
    }
    
    // �������
    float4 texColor = textureMap.Sample(samplerState, input.tc);

    // ������ɫ����
    float4 finalColor = saturate(diff) * texColor; // ������������
    return finalColor;
    //return diff * float4(1.0f, 1.0f, 1.0f,1.0f);
}
