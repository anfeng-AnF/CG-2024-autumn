cbuffer CBuf : register(b3)
{
    float4 color;
};

// 光照参数
float3 lightDir = normalize(float3(1.0, -1.0, -1.0)); // 光源方向
float4 lightColor = float4(1.0, 1.0, 1.0,1.0f); // 光源颜色（白光）
float3 ambientColor = float3(0.1, 0.1, 0.1); // 环境光颜色


float4 main(float4 pos : SV_POSITION, float3 normal : NORMAL) : SV_Target
{
    // 使用法线进行光照计算
    float3 lightDir = normalize(float3(-0.7, 3.0, -1.0)); // 示例光照方向
    float3 normalizedNormal = normalize(normal);
    float3 lightColor = float3(1.0, 1.0, 1.0); // 示例光照颜色
    float diffuse = max(dot(normalizedNormal, lightDir), 0.0) + float4(0.1f,0.1f,0.1f,0.1f);
    return float4(diffuse * lightColor, 1.0);
}


