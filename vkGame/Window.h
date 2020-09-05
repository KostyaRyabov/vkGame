#pragma once

class Window
{
public:
	Window();
	~Window();

	bool					Create();
	void					Open();
	void					Close();

	bool					Update();

	const HINSTANCE& GetInstance();
	const HWND& GetHandle();
private:
	bool					_run = false;

	HINSTANCE				_instance = GetModuleHandle(nullptr);
	HWND					_window = NULL;

	const wchar_t* _window_name = L"[test]";
};