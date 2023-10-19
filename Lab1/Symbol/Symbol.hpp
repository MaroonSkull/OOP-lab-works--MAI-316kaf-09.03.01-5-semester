#pragma once

#include <iostream>
#include <Windows.h>
#include <Global.hpp>


class Symbol {
private:
	int16_t offsetX_;
	int16_t offsetY_;
	char symbol_;
	int8_t color_;


public:
	Symbol(int16_t offsetX, int16_t offsetY, char symbol, int8_t color)
		: offsetX_{ offsetX }, offsetY_{ offsetY }, symbol_{ symbol }, color_{ color } {}

	void print(int16_t baseX, int16_t baseY) {
		print(baseX, baseY, symbol_);
	}

	void print(int16_t baseX, int16_t baseY, char symbol) {
		int16_t consoleX{ static_cast<int16_t>(baseX) + offsetX_ };
		int16_t consoleY{ static_cast<int16_t>(baseY) + offsetY_ };

		Global::setConsoleCursorPos(consoleX, consoleY);
		Global::setConsoleColor(color_);
		std::cout << symbol;
	}

	// getters & setters
	int16_t getXOffset() const {
		return offsetX_;
	}

	void setXOffset(int16_t offsetX) {
		offsetX_ = offsetX;
	}

	int16_t getYOffset() const {
		return offsetY_;
	}

	void setYOffset(int16_t offsetY) {
		offsetY_ = offsetY;
	}

	char getSymbol() const {
		return symbol_;
	}

	void setSymbol(char symbol) {
		symbol_ = { symbol };
	}

	int8_t getColor() const {
		return color_;
	}

	void setColor(int8_t color) {
		color_ = { color };
	}
};