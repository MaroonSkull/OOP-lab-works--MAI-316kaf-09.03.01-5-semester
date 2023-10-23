#pragma once

#include <windows.h>
#include <vector>


class Buffer {
public:
	// двумерный массив CHAR_INFO - структуры WinAPI, содержащей в себе символ и атрибуты (используем только атрибуты цвета)
	using type = std::vector< std::vector< CHAR_INFO > >;

	// Изменяет размеры буфера
	void resize(int16_t width, int16_t height);

	// Очищает буфер
	void clear();

	// Печатает весь буфер в консоль
	void print();

	type& getBuffer() {
		return buffer_;
	}
private:
	type buffer_;
	SMALL_RECT destinationRect_{};
};