#include "FloodFill.h"
#include <unordered_set>
#include "imguiManager.h"


namespace InputStates {
auto f = [](D3D11_MAPPED_SUBRESOURCE msrf, int width, int height, int x, int y, UINT32 fillColor) {
	// ������ӵ��Ƿ�����Ч��Χ��
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return;
	}

	// ��ȡͼ������ָ��
	UINT32* data = reinterpret_cast<UINT32*>(msrf.pData);

	// ��ȡ RowPitch
	size_t rowPitch = msrf.RowPitch / sizeof(UINT32);

	// ��ȡ���ӵ�ĳ�ʼ��ɫ
	UINT32 targetColor = data[y * rowPitch + x];

	// ������ӵ���ɫ�������ɫ��ͬ��ֱ�ӷ���
	if (targetColor == fillColor) {
		return;
	}

	// ���ڴ����������Ķ���
	std::queue<std::pair<int, int>> pixelQueue;
	pixelQueue.push({ x, y });

	// ��¼�Ѿ����ʵ�����
	std::unordered_set<int> visited;
	visited.insert(y * rowPitch + x);

	while (!pixelQueue.empty()) {
		auto [currentX, currentY] = pixelQueue.front();
		pixelQueue.pop();

		// �����ɫ
		data[currentY * rowPitch + currentX] = fillColor;

		// ��
		if (currentY > 0 && data[(currentY - 1) * rowPitch + currentX] == targetColor) {
			int upIndex = (currentY - 1) * rowPitch + currentX;
			if (visited.find(upIndex) == visited.end()) {
				pixelQueue.push({ currentX, currentY - 1 });
				visited.insert(upIndex);
			}
		}
		// ��
		if (currentY < height - 1 && data[(currentY + 1) * rowPitch + currentX] == targetColor) {
			int downIndex = (currentY + 1) * rowPitch + currentX;
			if (visited.find(downIndex) == visited.end()) {
				pixelQueue.push({ currentX, currentY + 1 });
				visited.insert(downIndex);
			}
		}
		// ��
		if (currentX > 0 && data[currentY * rowPitch + (currentX - 1)] == targetColor) {
			int leftIndex = currentY * rowPitch + (currentX - 1);
			if (visited.find(leftIndex) == visited.end()) {
				pixelQueue.push({ currentX - 1, currentY });
				visited.insert(leftIndex);
			}
		}
		// ��
		if (currentX < width - 1 && data[currentY * rowPitch + (currentX + 1)] == targetColor) {
			int rightIndex = currentY * rowPitch + (currentX + 1);
			if (visited.find(rightIndex) == visited.end()) {
				pixelQueue.push({ currentX + 1, currentY });
				visited.insert(rightIndex);
			}
		}
	}

	};

	void FloodFill::Enter()
	{
		wnd.mouse.Flush();
		wnd.Kbd.Flush();
	}

	void FloodFill::Update(float deltaTime)
	{
		while (auto mouseMsg = wnd.mouse.Read())
		{
			if (mouseMsg.value().GetType() == Mouse::Event::Type::LPress) {
				fillPos.push_back(mouseMsg.value().GetPos());
			}
		}
		while (auto c = wnd.Kbd.ReadKey())
		{
			switch (c.value().GetCode()) {
			case VK_ESCAPE:
				this->Machine->SetState(PERVIOUS_STATE);
				break;
			case VK_DELETE:
				fillPos.clear();
				break;
			case VK_BACK:
				if(fillPos.size()&&c.value().IsPress())
				fillPos.pop_back();
				break;
			}
		}
	}

	void FloodFill::Exit()
	{
		fillPos.clear();
	}

	void FloodFill::Draw()
	{
		static unsigned int color = 0xff0000ff;
		static auto colorf4 = ImGui::ColorConvertU32ToFloat4(color);

		// ��ʾ ImGui ����ɫ�༭�� (RGB)
		ImGui::Begin("fill settings");
		ImGui::ColorEdit3("fill Color", &colorf4.x); // ע��ֻ�༭ RGB ֵ��alpha ���ֲ���
		ImGui::End();

		// ������ɫ�����༭��� RGB ת�� U32������ԭ���� alpha ֵ
		color = ImGui::ColorConvertFloat4ToU32({ colorf4.z,colorf4.y,colorf4.x,colorf4.w });
		for (const auto& pos : fillPos) {
			wnd.Gfx().PostProcessingOnCPU(f,pos.first,pos.second,color);
		}
	}
}