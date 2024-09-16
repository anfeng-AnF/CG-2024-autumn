cbuffer CBuf : register(b3)
{
    float4 color;
};

// ���ղ���
float3 lightDir = normalize(float3(1.0, -1.0, -1.0)); // ��Դ����
float4 lightColor = float4(1.0, 1.0, 1.0,1.0f); // ��Դ��ɫ���׹⣩
float3 ambientColor = float3(0.1, 0.1, 0.1); // ��������ɫ


float4 main(float4 pos : SV_POSITION, float3 normal : NORMAL) : SV_Target
{
    // ʹ�÷��߽��й��ռ���
    float3 lightDir = normalize(float3(-0.7, 3.0, -1.0)); // ʾ�����շ���
    float3 normalizedNormal = normalize(normal);
    float3 lightColor = float3(1.0, 1.0, 1.0); // ʾ��������ɫ
    float diffuse = max(dot(normalizedNormal, lightDir), 0.0) + float4(0.1f,0.1f,0.1f,0.1f);
    return float4(diffuse * lightColor, 1.0);
}


