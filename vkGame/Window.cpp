#include "Settings.h"

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

bool Window::Create()
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
	window_class.lpszClassName = _window_name;

	if (!RegisterClassW(&window_class)) return false;

	DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	RECT window_rect = { 0,0,LONG(Settings::Window::width),LONG(Settings::Window::height) };

	AdjustWindowRectEx(&window_rect, style, FALSE, ex_style);

	_window = CreateWindowEx(0,
		_window_name,
		_window_name,
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top,
		NULL,
		NULL,
		_instance,
		NULL);

	if (!_window) return false;

	SetWindowLongPtr(_window, GWLP_USERDATA, (LONG_PTR)this);

	return true;
}

void Window::Open()
{
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

const HINSTANCE& Window::GetInstance()
{
	return _instance;
}

const HWND& Window::GetHandle()
{
	return _window;
}
