cbuffer CBuf : register(b3)
{
    float4 color;
};

// 光照参数
float3 lightDir = normalize(float3(1.0, -1.0, -1.0)); // 光源方向
float4 lightColor = float4(1.0, 1.0, 1.0, 1.0f); // 光源颜色（白光）
float3 ambientColor = float3(0.1, 0.1, 0.1); // 环境光颜色


float4 main(float4 pos : SV_POSITION, float3 normal : NORMAL) : SV_Target
{
    return float4(color);
}


