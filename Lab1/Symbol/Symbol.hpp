#pragma once

#include <random>
#include <iostream>
#include <Windows.h>


class Symbol {
private:
	HANDLE hConsole_ = GetStdHandle(STD_OUTPUT_HANDLE);

	uint16_t x_{};
	uint16_t y_{};
	uint16_t xOffset_{};
	uint16_t yOffset_{};
	bool epilepsy_;

	char symbol_{ 'd' };
	uint8_t color_{10}; // стандартный цвет, €рко-зелЄный
public:
	Symbol(uint16_t x, uint16_t y, uint16_t xOffset, uint16_t yOffset, bool epilepsy)
		: x_{ x }, y_{ y }, xOffset_{ xOffset }, yOffset_{ yOffset }, epilepsy_{ epilepsy } {
		// Seed with a real random value, if available
		std::random_device rd_;
		std::ranlux24_base engine_(rd_());
		std::uniform_int_distribution<uint32_t> uniformSymbolDist_(32, 255);
		symbol_ = static_cast<char>(uniformSymbolDist_(engine_));

		if (epilepsy_) {
			std::uniform_int_distribution<uint32_t> uniformColorDist_(0, 15);
			color_ = static_cast<uint8_t>(uniformColorDist_(engine_));
		}
	}

	void print() {
		COORD tl = { x_+xOffset_, y_-yOffset_ };
		SetConsoleCursorPosition(hConsole_, tl);
		SetConsoleTextAttribute(hConsole_, color_);
		std::cout << symbol_;
	}

	void setXY(uint16_t x, uint16_t y) {
		x_ = { x };
		y_ = { y };
	}
};