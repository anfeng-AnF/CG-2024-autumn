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
    int4 boneIdx[2] : BoneIndex; // 8 ����������
    float4 boneWeight[2] : BoneWeight; // 8 ������Ȩ��
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

    // ��һ������Ȩ�أ������Ҫ��
    float totalWeight =
        input.boneWeight[0].x + input.boneWeight[0].y + input.boneWeight[0].z + input.boneWeight[0].w +
        input.boneWeight[1].x + input.boneWeight[1].y + input.boneWeight[1].z + input.boneWeight[1].w;

    float4 skinnedPos = float4(0, 0, 0, 0);
    float4 skinnedNormal = float4(0, 0, 0, 0);
    
    if (totalWeight > 0.0)
    {
        // �������� 8 ������Ӱ��
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
        // ����Ƥ��ʹ��ԭʼλ�úͷ���
        skinnedPos = float4(input.pos, 1.0);
        skinnedNormal = float4(input.n, 0.0);
    }

    // �任���ü��ռ�
    output.pos = mul(skinnedPos, modelViewProj);
    // �任����ͼ�ռ�
    output.viewPos = mul(skinnedPos, modelView).xyz;
    // �任����һ������
    output.normal = skinnedNormal.xyz;
    // �������ߡ������ߺ���������
    output.tan =input.tan;
    output.bitan = input.bitan;
    output.tc = input.tc;

    output.modelView = modelView;
    return output;
}
