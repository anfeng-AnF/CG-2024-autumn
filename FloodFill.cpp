#include "FloodFill.h"
#include "imguiManager.h"
#include <unordered_set>
#include <stack>
#include <string>
#include <codecvt>
#include <locale>

// 函数将宽字符串转换为UTF-8字符串
std::string WideToUTF8(const std::wstring& wideStr) {
    // 计算需要的缓冲区大小
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), static_cast<int>(wideStr.length()), nullptr, 0, nullptr, nullptr);
    std::string utf8Str(size_needed, 0); // 创建UTF-8字符串的缓冲区
    WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), static_cast<int>(wideStr.length()), &utf8Str[0], size_needed, nullptr, nullptr);
    return utf8Str; // 返回转换后的UTF-8字符串
}

namespace InputStates {
    auto FloodFillFunc = [](D3D11_MAPPED_SUBRESOURCE msrf, int width, int height, int x, int y, UINT32 fillColor) {
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


    void DrawLineMidPoint(D3D11_MAPPED_SUBRESOURCE msrf, int width, int height,
        const FloodFill::ScreenPos& start, const FloodFill::ScreenPos& end,
        UINT32 Color) {
        // 将映射的资源转为可以操作的指针
        auto* buffer = static_cast<UINT32*>(msrf.pData);

        // 计算起始和结束坐标
        int x0 = start.x;
        int y0 = start.y;
        int x1 = end.x;
        int y1 = end.y;

        // 计算线的增量
        int dx = x1 - x0;
        int dy = y1 - y0;

        int sx = (dx > 0) ? 1 : -1;  // 步进方向
        int sy = (dy > 0) ? 1 : -1;  // 步进方向

        dx = std::abs(dx);
        dy = std::abs(dy);

        // 画点函数
        auto setPixel = [&](int x, int y) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                buffer[y * width + x] = Color;  // 注意行优先存储
            }
            };

        // 画线算法
        if (dx > dy) {
            int err = dx / 2;
            for (int i = 0; i <= dx; i++) {
                setPixel(x0, y0);
                err -= dy;
                if (err < 0) {
                    y0 += sy;
                    err += dx;
                }
                x0 += sx;
            }
        }
        else {
            int err = dy / 2;
            for (int i = 0; i <= dy; i++) {
                setPixel(x0, y0);
                err -= dx;
                if (err < 0) {
                    x0 += sx;
                    err += dy;
                }
                y0 += sy;
            }
        }
    }
    auto DrawLineMidPointFunc = [](D3D11_MAPPED_SUBRESOURCE msrf, int width, int height,
        std::vector<std::pair<FloodFill::ScreenPos, FloodFill::ScreenPos>> Lines,
        UINT32 Color) {
            for (const auto& line : Lines) {
                DrawLineMidPoint(msrf, width, height, line.first, line.second, Color);
            }
        };


    auto DrawLineBresenhamFunc = [](D3D11_MAPPED_SUBRESOURCE msrf, int width, int height, std::vector<std::pair<FloodFill::ScreenPos, FloodFill::ScreenPos>> Lines, UINT32 Color) {
        // 获取指向纹理数据的指针
        UINT32* pData = static_cast<UINT32*>(msrf.pData);

        // 遍历所有线段
        for (const auto& line : Lines) {
            // 获取线段的起点和终点
            FloodFill::ScreenPos p1 = line.first;
            FloodFill::ScreenPos p2 = line.second;

            int x1 = p1.x;
            int y1 = p1.y;
            int x2 = p2.x;
            int y2 = p2.y;

            // 计算线段的增量
            int dx = x2 - x1;
            int dy = y2 - y1;
            int sx = (dx >= 0) ? 1 : -1; // x 增量的符号
            int sy = (dy >= 0) ? 1 : -1; // y 增量的符号
            dx = std::abs(dx); // 获取绝对值
            dy = std::abs(dy); // 获取绝对值

            // 决策参数
            int err = dx - dy;

            // 绘制线段
            while (true) {
                // 边界检查
                if (x1 >= 0 && x1 < msrf.RowPitch / sizeof(UINT32) && y1 >= 0 && y1 < msrf.DepthPitch / sizeof(UINT32)) {
                    pData[y1 * (msrf.RowPitch / sizeof(UINT32)) + x1] = Color; // 更新颜色
                }

                // 如果已经到达终点，则退出
                if (x1 == x2 && y1 == y2) break;

                // 更新决策参数
                int err2 = err * 2;
                if (err2 > -dy) {
                    err -= dy;
                    x1 += sx; // x 增量
                }
                if (err2 < dx) {
                    err += dx;
                    y1 += sy; // y 增量
                }
            }
        }
        };

    int computeOutCode(int x, int y, const std::pair<FloodFill::ScreenPos, FloodFill::ScreenPos>& window) {
        int code = 0;
        if (x < window.first.x)    code |= 1; // 左
        if (x > window.second.x)   code |= 2; // 右
        if (y < window.first.y)    code |= 4; // 下
        if (y > window.second.y)   code |= 8; // 上
        return code;
    }

    auto CohenSutherlandClip = [](const std::vector<std::pair<FloodFill::ScreenPos, FloodFill::ScreenPos>>& Lines,
        std::pair<FloodFill::ScreenPos, FloodFill::ScreenPos> CroppingWindow) {
            std::vector<std::pair<FloodFill::ScreenPos, FloodFill::ScreenPos>> clippedLines;

            // 边框线段
            FloodFill::ScreenPos topLeft = CroppingWindow.first;
            FloodFill::ScreenPos bottomRight = CroppingWindow.second;
            // 添加窗口边框
            clippedLines.emplace_back(std::make_pair(topLeft, FloodFill::ScreenPos{ topLeft.x, bottomRight.y })); // 左边框
            clippedLines.emplace_back(std::make_pair(topLeft, FloodFill::ScreenPos{ bottomRight.x, topLeft.y })); // 上边框
            clippedLines.emplace_back(std::make_pair(FloodFill::ScreenPos{ bottomRight.x, topLeft.y }, bottomRight)); // 右边框
            clippedLines.emplace_back(std::make_pair(FloodFill::ScreenPos{ topLeft.x, bottomRight.y }, bottomRight)); // 下边框

            // 遍历每条待裁剪的线段
            for (const auto& line : Lines) {
                FloodFill::ScreenPos p1 = line.first;
                FloodFill::ScreenPos p2 = line.second;

                int outcode1 = computeOutCode(p1.x, p1.y, CroppingWindow);
                int outcode2 = computeOutCode(p2.x, p2.y, CroppingWindow);
                bool accept = false;

                while (true) {
                    if ((outcode1 == 0) && (outcode2 == 0)) {
                        // 两个端点都在裁剪区域内
                        accept = true;
                        break;
                    }
                    else if (outcode1 & outcode2) {
                        // 两个端点都在裁剪区域外
                        break;
                    }
                    else {
                        // 有一个端点在裁剪区域外
                        int outcodeOut = (outcode1 != 0) ? outcode1 : outcode2;
                        int x, y;

                        // 根据区域代码进行裁剪
                        if (outcodeOut & 8) { // 上
                            x = p1.x + (p2.x - p1.x) * (CroppingWindow.second.y - p1.y) / (p2.y - p1.y);
                            y = CroppingWindow.second.y;
                        }
                        else if (outcodeOut & 4) { // 下
                            x = p1.x + (p2.x - p1.x) * (CroppingWindow.first.y - p1.y) / (p2.y - p1.y);
                            y = CroppingWindow.first.y;
                        }
                        else if (outcodeOut & 2) { // 右
                            y = p1.y + (p2.y - p1.y) * (CroppingWindow.second.x - p1.x) / (p2.x - p1.x);
                            x = CroppingWindow.second.x;
                        }
                        else if (outcodeOut & 1) { // 左
                            y = p1.y + (p2.y - p1.y) * (CroppingWindow.first.x - p1.x) / (p2.x - p1.x);
                            x = CroppingWindow.first.x;
                        }

                        // 更新端点和区域代码
                        if (outcodeOut == outcode1) {
                            p1.x = x;
                            p1.y = y;
                            outcode1 = computeOutCode(p1.x, p1.y, CroppingWindow);
                        }
                        else {
                            p2.x = x;
                            p2.y = y;
                            outcode2 = computeOutCode(p2.x, p2.y, CroppingWindow);
                        }
                    }
                }

                // 只有当线段被接受时，才将其添加到裁剪后的线段列表中
                if (accept) {
                    clippedLines.emplace_back(std::make_pair(p1, p2));
                }
            }

            return clippedLines; // 返回裁剪后的线段，包括窗口边框
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
            HandleMouseMsg(mouseMsg);
        }
		while (auto c = wnd.Kbd.ReadKey())
		{
			switch (c.value().GetCode()) {
			case VK_ESCAPE:
				this->Machine->SetState(PERVIOUS_STATE);
				break;
			case VK_DELETE:
				fillPos.clear();
                if (OperationCounter[(int)InputStates::FloodFill::DrawLineMidpoint] == 0) {
                    Lines.clear();
                }
				break;
			case VK_BACK:
				if(fillPos.size()&&c.value().IsPress())
				fillPos.pop_back();
                if (OperationCounter[(int)InputStates::FloodFill::DrawLineMidpoint] == 0) {
                    Lines.pop_back();
                }
				break;
			}
		}
	}

	void FloodFill::Exit()
	{
		fillPos.clear();
        Lines.clear();
        std::vector<int> OperationCounter = std::vector<int>(5, 0);
	}

	void FloodFill::Draw()
	{
		static unsigned int color = 0xff0000ff;
		static auto colorf4 = ImGui::ColorConvertU32ToFloat4(color);
        // ImGui窗口代码
        ImGui::Begin("helps");
        ImGui::Text("%s", WideToUTF8(L"帮助:").c_str()); // Title
        ImGui::Separator(); // A separator line for better readability

        ImGui::Text("%s", WideToUTF8(L"1. 操作模式:").c_str());
        ImGui::BulletText("%s", WideToUTF8(L"洪水填充模式: 使用左键单击图像以填充颜色。").c_str());
        ImGui::BulletText("%s", WideToUTF8(L"中点绘制线段: 选择起点后，再次单击选择终点来绘制线段。").c_str());
        ImGui::BulletText("%s", WideToUTF8(L"Bresenham绘制线段: 选择起点后，再次单击选择终点来绘制线段。").c_str());
        ImGui::BulletText("%s", WideToUTF8(L"裁剪模式: 选择左上角，然后释放鼠标选择右下角定义裁剪区域。").c_str());

        ImGui::Text("%s", WideToUTF8(L"2. 颜色选择:").c_str());
        ImGui::BulletText("%s", WideToUTF8(L"使用颜色选择器更改当前操作的颜色。").c_str());

        ImGui::Text("%s", WideToUTF8(L"3. 键盘快捷键:").c_str());
        ImGui::BulletText("%s", WideToUTF8(L"ESC: 返回到上一个状态.").c_str());
        ImGui::BulletText("%s", WideToUTF8(L"DELETE: 清空所有填充点/直线.").c_str());
        ImGui::BulletText("%s", WideToUTF8(L"BACKSPACE: 撤销上一个填充点/直线 (如果有的话).").c_str());
        ImGui::BulletText("%s", WideToUTF8(L"左键: 用于选择点或定义窗口.").c_str());

        ImGui::Separator(); // Another separator
        ImGui::Text("%s", WideToUTF8(L"4. 注意事项:").c_str());
        ImGui::BulletText("%s", WideToUTF8(L"确保在图像的有效区域内进行操作，避免越界.").c_str());
        ImGui::End();


        ImGui::Begin("Image manipulation");
        ImGui::RadioButton("Flood Fill Mode", (int*)&OperatingMode, (int)FloodFillMode);
        ImGui::RadioButton("Draw Line (Midpoint)", (int*)&OperatingMode, (int)DrawLineMidpoint);
        ImGui::RadioButton("Draw Line (Bresenham)", (int*)&OperatingMode, (int)DrawLineBresenham);
        ImGui::RadioButton("Draw Clip Window", (int*)&OperatingMode, (int)ClipMode);
        if (ImGui::Button("reset Clip Windwo")) {
            ResetClipWindow();
        }
        ImGui::ColorEdit3("Color", &colorf4.x);
        color = ImGui::ColorConvertFloat4ToU32({ colorf4.z,colorf4.y,colorf4.x,colorf4.w });


        std::vector<std::pair<FloodFill::ScreenPos, FloodFill::ScreenPos>> ClippedLines;
        switch (OperatingMode)
        {
        case InputStates::FloodFill::FloodFillMode: {

            for (const auto& pos : fillPos) {
                wnd.Gfx().PostProcessingOnCPU(FloodFillFunc, pos.first, pos.second, color);
            }
        }
            break;
        case InputStates::FloodFill::DrawLineMidpoint:
            ClippedLines = CohenSutherlandClip(Lines, ClipWindow);
            wnd.Gfx().PostProcessingOnCPU(DrawLineMidPointFunc, ClippedLines, color);
            break;
        case InputStates::FloodFill::DrawLineBresenham:
            ClippedLines = CohenSutherlandClip(Lines, ClipWindow);
            wnd.Gfx().PostProcessingOnCPU(DrawLineBresenhamFunc, ClippedLines, color);
            break;
        case InputStates::FloodFill::ClipMode:
            ClippedLines = CohenSutherlandClip(Lines, ClipWindow);
            wnd.Gfx().PostProcessingOnCPU(DrawLineBresenhamFunc, ClippedLines, color);
            break;
        default:
            break;
        }
        ImGui::End();
	}
    void FloodFill::HandleMouseMsg(std::optional<Mouse::Event>& msg)
    {
        switch (OperatingMode)
        {
        case InputStates::FloodFill::FloodFillMode:
            if (msg.value().GetType() == Mouse::Event::Type::LPress) {
                fillPos.push_back(msg.value().GetPos());
            }
            break;
        case InputStates::FloodFill::DrawLineMidpoint:
        case InputStates::FloodFill::DrawLineBresenham: 
        {
            static std::pair<FloodFill::ScreenPos, FloodFill::ScreenPos> Line;
            if (0 == OperationCounter[(int)InputStates::FloodFill::DrawLineMidpoint]) {
                if (msg.value().GetType() == Mouse::Event::Type::LPress) {
                    Line.first = { msg.value().GetPosX() ,msg.value().GetPosY() };
                    OperationCounter[(int)InputStates::FloodFill::DrawLineMidpoint]++;
                    Lines.push_back(std::make_pair(Line.first, Line.first));
                }
            }
            else if (OperationCounter[(int)InputStates::FloodFill::DrawLineMidpoint] == 1) {
                if (msg.value().GetType() == Mouse::Event::Type::LPress) {
                    OperationCounter[(int)InputStates::FloodFill::DrawLineMidpoint] = 0;
                }
                Line.second = { msg.value().GetPosX() ,msg.value().GetPosY() };
                Lines.back() = Line;
            }
        }
            break;
        case InputStates::FloodFill::ClipMode:
            static std::pair<ScreenPos, ScreenPos> curWnd = ClipWindow;
            if (0 == OperationCounter[(int)InputStates::FloodFill::ClipMode]) {
                if (msg.value().GetType() == Mouse::Event::Type::LPress) {
                    curWnd.first = { msg.value().GetPosX() ,msg.value().GetPosY() };
                    curWnd.second = curWnd.first;
                    OperationCounter[(int)InputStates::FloodFill::ClipMode]++;
                }
            }
            else if (1 == OperationCounter[(int)InputStates::FloodFill::ClipMode]) {
                if (msg.value().GetType() == Mouse::Event::Type::LRelease) {
                    OperationCounter[(int)InputStates::FloodFill::ClipMode] = 0;
                }
                curWnd.second = { msg.value().GetPosX() ,msg.value().GetPosY() };
                ClipWindow.first.x = min(curWnd.first.x, curWnd.second.x);
                ClipWindow.first.y = min(curWnd.first.y, curWnd.second.y);
                ClipWindow.second.x = max(curWnd.first.x, curWnd.second.x);
                ClipWindow.second.y = max(curWnd.first.y, curWnd.second.y);
            }

            break;
        }
    }

    void FloodFill::ResetClipWindow()
    {
        ClipWindow.first = { 0,0 };
        ClipWindow.second = { wnd.GetWndSize().first,wnd.GetWndSize().second };
    }
}