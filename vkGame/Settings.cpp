#include "Settings.h"

void setDefault() {
    std::ofstream file("Settings.bin", std::ios::binary | std::ios::trunc);

    Settings::Window::width = 800;
    Settings::Window::height = 480;

    file << Settings::Window::width
        << Settings::Window::height;
}

void Settings::load() {
    std::ifstream file("Settings.bin", std::ios::binary);

    if (file.is_open()) {
        file >> Settings::Window::width
            >> Settings::Window::height;

        // добавить проверку, если файл в дальнейшем можно будет изменять напрямую
    }
    else {
        setDefault();
    }
}

void Settings::update() {
    std::ofstream file("Settings.bin", std::ios::binary | std::ios::trunc);

    file << Window::width
        << Window::height;
}