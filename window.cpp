#include "window.h"

static const wchar_t* DTA_WNDCLASS = L"anodewin";

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		// TODO: Unregister class here, probably want to store a pointer to the window class in the custom data
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window(int width, int height, LPCWSTR title)
	: m_hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = m_hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = DTA_WNDCLASS;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, L"Failed to register window class!", L"Fatal error", MB_ICONSTOP | MB_OK);
	}

	RECT rc;
	SetRect(&rc, 0, 0, width, height);
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	int clientWidth = rc.right - rc.left;
	int clientHeight = rc.bottom - rc.top;

	m_hwnd = CreateWindowEx(0, DTA_WNDCLASS, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, clientWidth, clientHeight, 0, nullptr, m_hInstance, 0);
	if (!m_hwnd)
	{
		MessageBox(nullptr, L"Failed to create a window!", L"Fatal error", MB_ICONSTOP | MB_OK);
	}

	ShowWindow(m_hwnd, SW_SHOWDEFAULT);
}

void Window::Update()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			m_shouldClose = true;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
