// 输入和输出结构体
struct PSIn
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position; // 如果需要在后续计算中使用
    matrix modelView : ModelView;
};

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

// 纹理采样器
Texture2D textureMap : register(t0);
SamplerState samplerState : register(s0);

float4 main(PSIn input) : SV_Target
{
	// fragment to light vector data
    const float3 vToL = lightPos - input.viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, input.normal));
	// reflected light vector
    const float3 w = input.normal * dot(vToL, input.normal);
    const float3 r = w * 2.0f - vToL;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffuseColor * diffuseIntensity) * 0.8f * pow(max(0.0f, dot(normalize(-r), normalize(input.viewPos))), 30.0f);
	// final color
    return float4(saturate((diffuse + ambient) * textureMap.Sample(samplerState, input.tc).rgb + specular), 1.0f);
}




