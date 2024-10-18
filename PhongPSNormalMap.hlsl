cbuffer LightCBuf
{
    float3 lightPos; // 光源位置（在视图坐标系中）
    float3 ambient; // 环境光颜色
    float3 diffuseColor; // 漫反射光颜色
    float diffuseIntensity; // 漫反射强度
    float attConst; // 衰减系数（常数部分）
    float attLin; // 衰减系数（线性部分）
    float attQuad; // 衰减系数（二次部分）
};

cbuffer ObjectCBuf
{
    float specularIntensity; // 镜面反射强度
    float specularPower; // 镜面反射的锐度
    bool normalMapEnabled; // 是否启用法线贴图
    float padding[1]; // 对齐填充
};

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;

float4 main(float3 viewPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    //normal map if normal mapping is enabled
    if (normalMapEnabled)
    {
        const float3x3 tanToView = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(n)
        );
        
        const float3 normalSample = nmap.Sample(splr, tc).xyz * 2.0f - 1.0f;
        
        n = normalize(mul(normalSample, tanToView));
    }

    const float3 vToL = lightPos - viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    const float3 viewDir = normalize(-viewPos);

    //Half-vector
    const float3 halfVector = normalize(dirToL + viewDir);
    const float3 specular = att * specularIntensity * pow(max(0.0f, dot(n, halfVector)), specularPower);
    float3 color = saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular);
    return float4(color, 1.0f);
}
