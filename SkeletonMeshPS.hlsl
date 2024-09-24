// 输入和输出结构体
struct PSIn
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position; // 如果需要在后续计算中使用
};

// 纹理采样器
Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

// 像素着色器
float4 main(PSIn input) : SV_Target
{
    // 计算简单的漫反射光照
    float3 lightDir = normalize(float3(0.0, 1.0, -1.0)); // 假设光源在(0, 1, -1)位置
    float3 norm = normalize(input.normal); // 法线归一化

    // 漫反射分量
    float diff = max(dot(norm, lightDir), 0.0);
    
    // 纹理采样
    float4 texColor = textureMap.Sample(samplerState, input.tc);

    // 最终颜色计算
    float4 finalColor = saturate(diff + 0.2) * texColor; // 仅考虑漫反射
    return finalColor;
    return diff * float4(1.0f, 1.0f, 1.0f,1.0f);
}
