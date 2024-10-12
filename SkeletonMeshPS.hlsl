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

float4 main(PSIn input) : SV_Target
{
    float3 lightDir = normalize(float3(0.0, 1.0, -1.0));
    float3 norm = normalize(input.normal);

    // 计算漫反射光照分量
    float dotVal = dot(norm, lightDir);

    // 定义阴影过渡带边界
    float shadowEdge0 = 0.4;
    float shadowEdge1 = 0.5;
    float diff = smoothstep(shadowEdge0, shadowEdge1, dotVal) * 0.4 + 0.4; // 使 diff 从 0.4 到 0.8 平滑过渡

    // 定义颜色过渡边界区间
    float colorEdge0 = 0.5;
    float colorEdge1 = 0.4;
    float transitionFactor = 0.0;

    // 如果 dotVal 在颜色过渡区间内，则计算过渡因子
    if (dotVal > colorEdge1 && dotVal < colorEdge0)
    {
        transitionFactor = smoothstep(colorEdge0, colorEdge1, dotVal)*0.5+0.3; // 调整边界顺序以反向过渡
    }

    // 定义过渡颜色
    float4 transitionColor = float4(0.8, 0.6, 0.5, 1.0)*0.5;

    // 纹理采样
    float4 texColor = textureMap.Sample(samplerState, input.tc);

    // 计算基础颜色
    float4 baseColor = diff * texColor;

    // 在过渡区域内混合基础颜色和过渡颜色
    float4 finalColor = lerp(baseColor, transitionColor, transitionFactor);

    return finalColor;
}




