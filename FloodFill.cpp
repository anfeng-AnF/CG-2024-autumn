#include "FloodFill.h"
#include "imguiManager.h"
#include <unordered_set>
#include <stack>


namespace InputStates {
    auto f = [](D3D11_MAPPED_SUBRESOURCE msrf, int width, int height, int x, int y, UINT32 fillColor) {
        // 检查种子点是否在有效范围内
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return;
        }

        // 获取图像数据指针
        UINT32* data = reinterpret_cast<UINT32*>(msrf.pData);

        // 获取 RowPitch
        size_t rowPitch = msrf.RowPitch / sizeof(UINT32);

        // 获取种子点的初始颜色
        UINT32 targetColor = data[y * rowPitch + x];

        // 如果种子点颜色与填充颜色相同，直接返回
        if (targetColor == fillColor) {
            return;
        }

        // 定义用于存储待处理区段的栈
        std::stack<std::pair<int, int>> pixelStack;
        pixelStack.push({ x, y });

        while (!pixelStack.empty()) {
            auto [startX, startY] = pixelStack.top();
            pixelStack.pop();

            // 跳过已经被填充的像素
            if (data[startY * rowPitch + startX] != targetColor) {
                continue;
            }

            // 向左和向右扩展，填充当前扫描线
            int leftX = startX;
            int rightX = startX;

            // 向左扩展
            while (leftX > 0 && data[startY * rowPitch + (leftX - 1)] == targetColor) {
                leftX--;
            }

            // 向右扩展
            while (rightX < width - 1 && data[startY * rowPitch + (rightX + 1)] == targetColor) {
                rightX++;
            }

            // 填充当前行
            for (int i = leftX; i <= rightX; ++i) {
                data[startY * rowPitch + i] = fillColor;
            }

            // 处理上方和下方扫描线（只对未填充区域递归）
            if (startY > 0) {
                for (int i = leftX; i <= rightX; ++i) {
                    if (data[(startY - 1) * rowPitch + i] == targetColor) {
                        pixelStack.push({ i, startY - 1 });
                    }
                }
            }

            if (startY < height - 1) {
                for (int i = leftX; i <= rightX; ++i) {
                    if (data[(startY + 1) * rowPitch + i] == targetColor) {
                        pixelStack.push({ i, startY + 1 });
                    }
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

		// 显示 ImGui 的颜色编辑器 (RGB)
		ImGui::Begin("fill settings");
		ImGui::ColorEdit3("fill Color", &colorf4.x); // 注意只编辑 RGB 值，alpha 保持不变
		ImGui::End();

		// 更新颜色，将编辑后的 RGB 转回 U32，保留原本的 alpha 值
		color = ImGui::ColorConvertFloat4ToU32({ colorf4.z,colorf4.y,colorf4.x,colorf4.w });
		for (const auto& pos : fillPos) {
			wnd.Gfx().PostProcessingOnCPU(f,pos.first,pos.second,color);
		}
	}
}