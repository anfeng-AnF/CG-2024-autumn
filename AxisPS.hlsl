struct PSIn
{
    float4 pos : SV_Position; // ����λ�ã�������ɫ�������λ��
    float4 color : COLOR; // ������ɫ��������ɫ���������ɫ
};

float4 main(PSIn p) : SV_Target
{
    return p.color; // ������ɫ��Ϊ���յ��������
}
