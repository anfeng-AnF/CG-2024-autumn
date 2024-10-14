cbuffer CBuf :register(b0)
{
    matrix modelView;
    matrix modelViewProj;
};

cbuffer BonesTransform : register(b1)
{
    matrix bones[1024];
};

struct VSIn
{
    float3 pos : Position;
    float3 n : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    int4 boneIdx[2] : BoneIndex; // 8 个骨骼索引
    float4 boneWeight[2] : BoneWeight; // 8 个骨骼权重
};

struct VSOut
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
    matrix modelView : ModelView;
};

VSOut main(VSIn input)
{
    VSOut output;

    // 归一化骨骼权重（如果需要）
    float totalWeight =
        input.boneWeight[0].x + input.boneWeight[0].y + input.boneWeight[0].z + input.boneWeight[0].w +
        input.boneWeight[1].x + input.boneWeight[1].y + input.boneWeight[1].z + input.boneWeight[1].w;

    float4 skinnedPos = float4(0, 0, 0, 0);
    float4 skinnedNormal = float4(0, 0, 0, 0);
    
    if (totalWeight > 0.0)
    {
        // 遍历所有 8 个骨骼影响
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                int boneIndex = input.boneIdx[i][j];
                float boneWeight = input.boneWeight[i][j];

                if (boneIndex != -1 && boneWeight > 0.0)
                {
                    float normalizedWeight = boneWeight / totalWeight;
                    skinnedPos += normalizedWeight * mul(float4(input.pos, 1.0), bones[boneIndex]);
                    skinnedNormal += normalizedWeight * mul(float4(input.n, 0.0), bones[boneIndex]);
                }
            }
        }
    }
    else
    {
        // 无蒙皮；使用原始位置和法线
        skinnedPos = float4(input.pos, 1.0);
        skinnedNormal = float4(input.n, 0.0);
    }

    // 变换到裁剪空间
    output.pos = mul(skinnedPos, modelViewProj);
    // 变换到视图空间
    output.viewPos = mul(skinnedPos, modelView).xyz;
    // 变换并归一化法线
    output.normal = skinnedNormal.xyz;
    // 传递切线、副切线和纹理坐标
    output.tan =input.tan;
    output.bitan = input.bitan;
    output.tc = input.tc;

    output.modelView = modelView;
    return output;
}
