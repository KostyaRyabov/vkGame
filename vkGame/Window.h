#pragma once

class Window
{
public:
	Window(uint32_t size_x, uint32_t size_y, const wchar_t* name);
	~Window();

	void Open();
	void Close();

	bool Update();

	HINSTANCE& GetInstance();
	HWND& GetHandle();
private:
	bool			_run			= false;

	uint32_t		_surface_size_x = 800;
	uint32_t		_surface_size_y = 480;

	HINSTANCE		_instance		= GetModuleHandle(nullptr);
	HWND			_window			= NULL;

	const wchar_t* _window_name;
};

