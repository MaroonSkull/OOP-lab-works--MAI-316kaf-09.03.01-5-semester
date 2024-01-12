#pragma once

#include <windows.h>
#include <vector>


class Buffer {
public:
	// Изменяет размеры буфера
	void resize(int16_t width, int16_t height);

	// Очищает буфер
	void clear();

	// Печатает весь буфер в консоль
	void print();

	// Предоставляет доступ к конкретному символу
	CHAR_INFO& getChar(int16_t consoleX, int16_t consoleY);
private:
	// вектор линейно хранит двумерный массив CHAR_INFO - структуры WinAPI, содержащей в себе символ и атрибуты (используем только атрибуты цвета)
	std::vector< CHAR_INFO > buffer_;
	COORD Sizes_{};
	SMALL_RECT destinationRect_{};
};