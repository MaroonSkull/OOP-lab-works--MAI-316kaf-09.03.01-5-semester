#pragma once

#include <Symbol.hpp>
#include <Global.hpp>

#include <iostream>
#include <list>
#include <functional>



class Line {
private:
	std::list<Symbol> Symbols_;
	// базовые координаты, от которых начинается линия
	double x_{};
	double y_{};

	int8_t xOffsetCounter_{};
	int8_t yOffsetCounter_{};

	uint16_t width_;
	uint16_t height_;

	bool epilepsy_;

	uint8_t length_{};

	char generateRandomSymbol() {
		return static_cast<char>(Global::getRandomUniformIntDistribution(32, 255));
	}

	uint8_t generateColor() {
		// цвет рандомный, если установлен флаг эпилепсии, ярко-зелёный (№10) по-умолчанию
		return epilepsy_ ? static_cast<uint8_t>(Global::getRandomUniformIntDistribution(1, 15)) : 10;
	}

public:
	Line(uint16_t width, uint16_t height, uint8_t length, bool epilepsy)
		: width_{ width }, height_{ height }, length_{ length }, epilepsy_{ epilepsy }
	{
		// тут x_ и y_ зависит от направления
		x_ = Global::getRandomUniformIntDistribution(static_cast<decltype(width_)>(0), width_);
		//y_ = Global::getRandomUniformIntDistribution(static_cast<decltype(height_)>(0), height_);
		y_ = 0;
	}

	// Добавляет ровно один символ в линию
	void addSymbol() {
		// Выбираем рандомный символ
		char symbol{ generateRandomSymbol() };

		// и задаём цвет
		uint8_t color{ generateColor() };

		Symbols_.push_back(Symbol(xOffsetCounter_, yOffsetCounter_, symbol, color));
	}

	void move(double distance) {
		// здесь поведение зависит от варианта
		
		// Получаем текущую позицию начала линии
		uint16_t y{ static_cast<uint16_t>(y_) };
		// Накапливаем смещение
		y_ += distance;

		// Вычисляем, на сколько позиций нам надо сместиться
		uint16_t steps = static_cast<uint16_t>(y_) - y;


		// Если целочисленно нам пора бы уже сместиться
		if (steps != 0) {
			// Если мы всё ещё не полностью добавили все символы линии
			for (; Symbols_.size() < length_ && steps > 0; steps--) {
				// Играемся с отступами как нам надо
				//xOffsetCounter_--;
				yOffsetCounter_++;
				// Добавляем символ
				addSymbol();
				y_--;
			}
			// Мы уже добавили всё, что только могли, теперь остаётся только перемещать уже существующие символы
			while (steps > 0) {
				auto currentIt = Symbols_.begin();
				for (; std::next(currentIt) != Symbols_.end(); currentIt++) {

					currentIt->setColor(std::next(currentIt)->getColor());
					currentIt->setSymbol(std::next(currentIt)->getSymbol());
					// Так же необходимо сместить в обратную сторону символы
					// и их цвета, сгенерировать последнему новый символ
				}
				currentIt->setSymbol(generateRandomSymbol());
				currentIt->setColor(generateColor());
				steps--;
			}
		}
	}

	uint16_t getX() const {
		return static_cast<uint16_t>(x_);
	}

	uint16_t getY() const {
		return static_cast<uint16_t>(y_);
	}

	// Печатает те символы, которые находятся внутри экрана
	void print(uint16_t width, uint16_t height) {
		// Обновляем данные о высоте и ширине экрана
		width_ = { width };
		height_ = { height };

		// Проходим по всем символам
		for (auto& Node : Symbols_) {
			// Узнаём позицию, где фактически должен быть отображён символ
			uint16_t xSymbolPosition{ static_cast<uint16_t>(x_ + Node.getXOffset()) };
			uint16_t ySymbolPosition{ static_cast<uint16_t>(y_ + Node.getYOffset()) };

			// Если эта позиция находится в пределах экрана
			if (xSymbolPosition == std::clamp(xSymbolPosition, static_cast<uint16_t>(0), width_) &&
				ySymbolPosition == std::clamp(ySymbolPosition, static_cast<uint16_t>(0), height_))
				// То печатаем символ
				Node.print(x_, y_);
		}
	}
};