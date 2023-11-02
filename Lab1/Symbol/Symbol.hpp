#pragma once


#include <Windows.h>

#include <Global.hpp>



class Symbol {
private:
	int offsetX;
	int offsetY;
	char symbol;
	int color;

	void setConsoleCursorPos(int x, int y) const;

	void setConsoleColor(int color) const;

public:
	Symbol(int offsetX, int offsetY, char symbol, int color)
		: offsetX{ offsetX }, offsetY{ offsetY }, symbol{ symbol }, color{ color } {}

	void print(int x, int y);

	void print(int x, int y, char symbol);

	// Геттеры и сеттеры расположены в хэдере, чтобы инлайнить функцию с простой логикой
	// это позволяет компилятору оптимизировать код, вовсе убирая накладные расходы на вызов функции
	int getXOffset() const {
		return offsetX;
	}

	void setXOffset(int offsetX) {
		this->offsetX = offsetX;
	}

	int getYOffset() const {
		return offsetY;
	}

	void setYOffset(int offsetY) {
		this->offsetY = offsetY;
	}

	char getSymbol() const {
		return symbol;
	}

	void setSymbol(char symbol) {
		this->symbol = symbol;
	}

	int getColor() const {
		return color;
	}

	void setColor(int color) {
		this->color = color;
	}
};