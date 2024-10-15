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
struct VSOut
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
    matrix modelview : ModelViewr;
};

float4 main(VSOut input) : SV_Target
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
        transitionFactor = smoothstep(colorEdge0, colorEdge1, dotVal) * 0.5 + 0.3; // 调整边界顺序以反向过渡
    }

    // 定义过渡颜色
    float4 transitionColor = float4(0.8, 0.6, 0.5, 1.0) * 0.5;

    // 纹理采样
    float4 texColor = tex.Sample(splr, input.tc);

    // 计算基础颜色
    float4 baseColor = diff * texColor;

    //判断是否是 描边区域
    float3 viewDir = normalize(-input.viewPos);
    float3 viewNormal = mul(float4(norm, 1.0f), input.modelview).xyz - mul(float4(0.0f, 0.0f, 0.0f, 1.0f), input.modelview).xyz;
    float edgeIntensity = 1 - pow(1 - dot(viewNormal, viewDir) / length(viewNormal), 20);
    if (abs(edgeIntensity) > 0.2)
    {
       //edgeIntensity = 1.0f;
    }
    float4 finalColor = lerp(baseColor, transitionColor, transitionFactor);

    return finalColor;
}