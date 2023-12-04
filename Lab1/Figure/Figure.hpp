#pragma once


#include <algorithm>
#include <list>

#include <Global.hpp>
#include <Buffer.hpp>
#include <Symbol.hpp>



class Figure {
protected:
	std::list<Symbol> Symbols_;
	// базовые координаты, от которых начинается фигура
	double x_{};
	double y_{};

	int8_t xOffsetCounter_{};
	int8_t yOffsetCounter_{};

	bool epilepsy_{};
	int8_t figureColor_{};

	int16_t width_{};
	int16_t height_{};

	char generateSymbol(bool isSpace = false) {
		return isSpace ? ' ' : static_cast<char>(Global::getRandomUniformDistribution(33, 126));;
	}

	int8_t generateColor() {
		// цвет рандомный, если установлен флаг эпилепсии для символа, Цвет константно-рандомный, если установлен флаг эпилепсии для Линии. Ярко-зелёный (№10) по-умолчанию

		if constexpr (Global::myEpilepsyType == Global::EpilepsyType::symbol)
			return epilepsy_ ? static_cast<int8_t>(Global::getRandomUniformDistribution(1, 15)) : 10;
		else if constexpr (Global::myEpilepsyType == Global::EpilepsyType::line)
			return epilepsy_ ? figureColor_ : 10;
	}

public:
	Figure(int16_t width, int16_t height, bool epilepsy) : width_{ width }, height_{ height }, epilepsy_{ epilepsy } {}

        // Добавляет ровно один символ в линию
	void addSymbol(bool isSpace = false) {
		// Если это не пробел, то выбираем рандомный символ
		char symbol{ generateSymbol(isSpace) };

		// и задаём цвет
		int8_t color{ generateColor() };

		Symbols_.push_back(Symbol(xOffsetCounter_, yOffsetCounter_, symbol, color));
	}


	virtual void move(Buffer& Buff, double distance) = 0;

	// Печатает те символы, которые находятся внутри экрана
	void print(Buffer &Buff, int16_t width, int16_t height) {
		// Обновляем данные о высоте и ширине экрана
		width_ = { width };
		height_ = { height };

		auto printIfVisible = [this, &Buff](auto &Node) {
			// Округляем позицию начала линии до целых
			int16_t xHeadPosition{ static_cast<int16_t>(x_) };
			int16_t yHeadPosition{ static_cast<int16_t>(y_) };

			// Узнаём позицию, где фактически должен быть отображён символ
			int16_t xSymbolPosition{ static_cast<int16_t>(xHeadPosition + Node.getXOffset()) };
			int16_t ySymbolPosition{ static_cast<int16_t>(yHeadPosition + Node.getYOffset()) };

			// Если эти позиции находятся в пределах экрана
			if (xSymbolPosition == std::clamp(xSymbolPosition, static_cast<int16_t>(0), width_) &&
				ySymbolPosition == std::clamp(ySymbolPosition, static_cast<int16_t>(0), height_) &&
				xHeadPosition == std::clamp(xHeadPosition, static_cast<int16_t>(0), width_) &&
				yHeadPosition == std::clamp(yHeadPosition, static_cast<int16_t>(0), height_))
				// То печатаем символ
				Node.print(Buff, xSymbolPosition, ySymbolPosition);
		};

		// Проходим по всем символам
		std::ranges::for_each(Symbols_, printIfVisible);
	}

	int16_t getX() const {
		return static_cast<int16_t>(x_);
	}

	int16_t getY() const {
		return static_cast<int16_t>(y_);
	}
};