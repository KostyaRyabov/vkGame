#include "pch.h"

LRESULT CALLBACK WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

	switch (message) {
	case WM_CLOSE:
		window->Close();
		return 0;
	case WM_SIZE:
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

Window::Window() {
}

Window::~Window()
{
	if (_run) Close();
}

void Window::Open(uint32_t size_x, uint32_t size_y, const wchar_t* name)
{
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = WndProc;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = _instance;
	window_class.hIcon = LoadIcon(_instance, IDI_APPLICATION);
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	window_class.lpszMenuName = NULL;
	window_class.lpszClassName = name;

	_surface_size_x = size_x;
	_surface_size_y = size_y;
	_window_name = name;

	if (!RegisterClassW(&window_class)) {
		std::cout << "error - window is not loaded!";
	}

	DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	RECT window_rect = { 0,0,LONG(_surface_size_x),LONG(_surface_size_y) };

	AdjustWindowRectEx(&window_rect, style, FALSE, ex_style);

	_window = CreateWindowEx(0,
		name,
		name,
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top,
		NULL,
		NULL,
		_instance,
		NULL);

	if (!_window) {
		std::cout << "error - window don't created!";
	}

	SetWindowLongPtr(_window, GWLP_USERDATA, (LONG_PTR)this);

	_run = true;

	ShowWindow(_window, SW_SHOW);
	SetForegroundWindow(_window);
	SetFocus(_window);
}

void Window::Close()
{
	_run = false;

	DestroyWindow(_window);
	UnregisterClass(_window_name, _instance);
}

bool Window::Update()
{
	MSG msg;
	if (PeekMessage(&msg, _window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return _run;
}

HINSTANCE& Window::GetInstance()
{
	return _instance;
}

HWND& Window::GetHandle()
{
	return _window;
}
