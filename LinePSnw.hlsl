cbuffer CBuf : register(b3)
{
    float4 color;
};

// ���ղ���
float3 lightDir = normalize(float3(1.0, -1.0, -1.0)); // ��Դ����
float4 lightColor = float4(1.0, 1.0, 1.0, 1.0f); // ��Դ��ɫ���׹⣩
float3 ambientColor = float3(0.1, 0.1, 0.1); // ��������ɫ


float4 main(float4 pos : SV_POSITION, float3 normal : NORMAL) : SV_Target
{
    return float4(color);
}


