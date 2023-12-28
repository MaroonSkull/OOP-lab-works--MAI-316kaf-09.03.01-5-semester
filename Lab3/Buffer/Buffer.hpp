#pragma once

#include <ncurses.h>
#include <ArrayList.hpp>

class Buffer {
public:
	// Изменяет размеры буфера
	void resize(int16_t width, int16_t height);

	// Очищает буфер
	void clear();

	// Печатает весь буфер в консоль
	void print();
	
	// Пара, хранит символ и его foreground цвет
	using CHAR_INFO = std::pair<char, attr_t>;
	
	// Предоставляет доступ к конкретному символу
	CHAR_INFO& getChar(int16_t consoleX, int16_t consoleY);
private:
	// вектор линейно хранит двумерный массив CHAR_INFO - структуры WinAPI, содержащей в себе символ и атрибуты (используем только атрибуты цвета)
	ArrayList< CHAR_INFO > buffer_;
	int16_t x_{};
	int16_t y_{};
	int16_t size_{};
	//SMALL_RECT destinationRect_{};
};