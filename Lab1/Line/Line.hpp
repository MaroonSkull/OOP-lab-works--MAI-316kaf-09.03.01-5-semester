#pragma once

#include <iostream>
#include <Symbol.hpp>
#include <list>
#include <random>
#include <functional>



class Line {
private:
	std::list<Symbol> Symbols_;
	// базовые координаты, от которых начинается линия
	double x_{};
	double y_{};

	uint8_t xOffsetCounter_{};
	uint8_t yOffsetCounter_{};

	uint16_t width_;
	uint16_t height_;

	bool epilepsy_;

	uint8_t length_{};
public:
	Line(uint8_t length, uint16_t width, uint16_t height, bool epilepsy)
		: length_{ length }, y_{ static_cast<double>(height) }, width_{ width }, height_{ height }, epilepsy_{ epilepsy }
	{
		std::random_device rd_;
		std::ranlux24_base engine_(rd_());
		std::uniform_int_distribution<uint32_t> uniformDist_(0, width_);
		x_ = uniformDist_(engine_);
		move(0.); // первый элемент линии
	}

	void addSymbol() {
		if (Symbols_.size() >= length_) {
			Symbols_.pop_front();
			xOffsetCounter_--;
			yOffsetCounter_--;
		}
		for (auto& Node : Symbols_) {

			// Seed with a real random value, if available
			std::random_device rd_;
			std::ranlux24_base engine_(rd_());
			std::uniform_int_distribution<uint32_t> uniformSymbolDist_(32, 255);
			char symbol = static_cast<char>(uniformSymbolDist_(engine_));

			if (epilepsy_) {
				std::uniform_int_distribution<uint32_t> uniformColorDist_(0, 15);
				uint8_t color = static_cast<uint8_t>(uniformColorDist_(engine_));
			}

			// тут надо мб у следующей ноды взять символ и цвет? 
			//Node.setXY(x_, y_);
		}
		Symbols_.push_back(Symbol(xOffsetCounter_++, yOffsetCounter_++));
	}

	void move(double dx) {
		uint16_t y{ static_cast<uint16_t>(y_) };
		y_ -= dx; // здесь поведение зависит от варианта
		if (static_cast<uint16_t>(y_) - y != 0)
			addSymbol();
	}

	uint8_t getXOffset() const {
		return xOffsetCounter_;
	}

	uint16_t getYOffset() const {
		return yOffsetCounter_;
	}

	void print() {
		for (auto& Node : Symbols_)
			Node.print(0, 0);
	}
};