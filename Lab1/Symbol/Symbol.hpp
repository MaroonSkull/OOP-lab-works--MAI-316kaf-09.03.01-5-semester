#pragma once

#include <iostream>
#include <Windows.h>
#include <Global.hpp>


class Symbol {
private:
	int16_t offsetX_;
	int16_t offsetY_;
	char symbol_;
	uint8_t color_;


public:
	Symbol(int16_t offsetX, int16_t offsetY, char symbol, uint8_t color)
		: offsetX_{ offsetX }, offsetY_{ offsetY }, symbol_{ symbol }, color_{ color } {}

	void print(uint16_t baseX, uint16_t baseY) {
		print(baseX, baseY, symbol_);
	}

	void print(uint16_t baseX, uint16_t baseY, char symbol) {
		int16_t consoleX{ static_cast<int16_t>(baseX) + offsetX_ };
		int16_t consoleY{ static_cast<int16_t>(baseY) + offsetY_ - 1 };
		if (consoleX < 0 || consoleY < 0) throw std::logic_error{ "Это конец" };
		Global::setConsoleCursorPos(consoleX, consoleY);
		Global::setConsoleColor(color_);
		std::cout << symbol;
		//printf("%c", symbol_);
		//DWORD dwBytesWritten = 0;
		//WriteConsoleOutputCharacter(Global::hConsole, &symbol_, 1, tl, &dwBytesWritten);
		Global::setConsoleColor(0);
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

	uint8_t getColor() const {
		return color_;
	}

	void setColor(uint8_t color) {
		color_ = { color };
	}
};