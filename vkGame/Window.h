#pragma once

class Window
{
public:
	Window();
	~Window();

	bool			Create(uint32_t size_x, uint32_t size_y, const wchar_t* name);
	void			Open();
	void			Close();

	bool			Update();

	HINSTANCE&		GetInstance();
	HWND&			GetHandle();
	VkExtent2D&		GetSize();
private:
	bool			_run			= false;

	VkExtent2D		_surface_size;

	HINSTANCE		_instance		= GetModuleHandle(nullptr);
	HWND			_window			= NULL;

	const wchar_t* _window_name;
};

