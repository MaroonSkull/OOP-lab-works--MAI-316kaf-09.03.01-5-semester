#pragma once

#include <iostream>
#include <Windows.h>


class Symbol {
private:
	uint16_t offsetX_;
	uint16_t offsetY_;
	char symbol_;
	uint8_t color_;

	bool isVisible_{ false };

	HANDLE hConsole_ = GetStdHandle(STD_OUTPUT_HANDLE);

public:
	Symbol(uint16_t offsetX, uint16_t offsetY, char symbol = 'G', uint8_t color = 10)
		: offsetX_{ offsetX }, offsetY_{ offsetY }, symbol_{ symbol }, color_{ color } {}

	void print(uint16_t baseX, uint16_t baseY) {
		if (!isVisible_)
			return;

		COORD tl = { baseX+offsetX_, baseY+offsetY_ };
		SetConsoleCursorPosition(hConsole_, tl);
		SetConsoleTextAttribute(hConsole_, color_);
		std::cout << symbol_;
	}

	char getSymbol() const {
		return symbol_;
	}

	void setSymbol(char symbol) {
		symbol_ = { symbol };
	}

	uint8_t getColor() const {
		return color_;
	}

	void setColor(uint8_t color) {
		color_ = { color };
	}
};