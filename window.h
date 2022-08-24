#pragma once

#include <Windows.h>
#include <optional>

class Window
{
public:
	Window(int width, int height, LPCWSTR title);

	void Update();
	bool ShouldClose() const { return m_shouldClose; };

	HWND GetWindowHandle() const { return m_hwnd; }
private:
	bool m_shouldClose{ false };
	HWND m_hwnd;
	HINSTANCE m_hInstance;
};
