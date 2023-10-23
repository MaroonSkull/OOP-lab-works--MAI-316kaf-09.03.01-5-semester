#pragma once


#include <iostream>
#include <list>

#include <Global.hpp>
#include <Symbol.hpp>



class Line {
private:
	std::list<Symbol> Symbols_;
	// базовые координаты, от которых начинается линия
	double x_{};
	double y_{};

	int8_t xOffsetCounter_{};
	int8_t yOffsetCounter_{};

	bool toggle_{};

	int16_t width_{};
	int16_t height_{};
	int8_t length_{};
	bool epilepsy_{};
	int8_t lineColor_{}; // можно вынести в LineTraits, но мне не жалко лишний байт

	char generateSymbol(bool isSpace = false);

	int8_t generateColor();

public:
	Line(int16_t width, int16_t height, int8_t length, bool epilepsy);

	// Добавляет ровно один символ в линию
	void addSymbol(bool isSpace = false);

	void move(double distance);

	// Печатает те символы, которые находятся внутри экрана
	void print(int16_t width, int16_t height);

	// геттеры и сеттеры можно указывать в заголовочных файлах
	int16_t getX() const {
		return static_cast<int16_t>(x_);
	}

	int16_t getY() const {
		return static_cast<int16_t>(y_);
	}
};