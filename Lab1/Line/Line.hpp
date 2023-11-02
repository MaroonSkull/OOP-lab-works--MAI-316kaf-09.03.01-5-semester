#pragma once


#include <list>

#include <Symbol.hpp>



class Line {
private:
	std::list<Symbol> Symbols;
	// базовые координаты, от которых начинается линия
	double x = 0.0;
	double y = 0.0;

	int xOffsetCounter = 0;
	int yOffsetCounter = 0;

	bool toggle = false;

	int width = 0;
	int height = 0;
	int length = 0;
	bool epilepsy = false;
	int lineColor = 0;

	char generateSymbol(bool isSpace = false);

	int generateColor();

public:
	Line(int width, int height, int length, bool epilepsy);

	// Проверка, находится ли координата внутри окна
	bool isCoordInsideFrame(int x, int y) const;

	// Добавляет ровно один символ в линию
	void addSymbol(bool isSpace = false);

	void move(double distance);

	// Печатает те символы, которые находятся внутри экрана
	void print(int width, int height);

	// геттеры можно указывать в заголовочных файлах
	int getX() const {
		return x;
	}

	int getY() const {
		return y;
	}
};