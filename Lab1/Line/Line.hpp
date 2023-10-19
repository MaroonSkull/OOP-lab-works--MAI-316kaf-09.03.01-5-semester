#pragma once

#include <Symbol.hpp>
#include <Global.hpp>

#include <iostream>
#include <list>
#include <functional>


template <Global::Direction _Direction> 
class Line {
private:
	std::list<Symbol> Symbols_;
	// базовые координаты, от которых начинается линия
	double x_{};
	double y_{};

	int8_t xOffsetCounter_{};
	int8_t yOffsetCounter_{};

	int16_t width_;
	int16_t height_;
	int8_t length_{};
	bool epilepsy_;


	char generateRandomSymbol() {
		char symbol = static_cast<char>(Global::getRandomUniformIntDistribution(33, 126));
		return symbol;
	}

	int8_t generateColor() {
		// цвет рандомный, если установлен флаг эпилепсии, ярко-зелёный (№10) по-умолчанию
		return epilepsy_ ? static_cast<int8_t>(Global::getRandomUniformIntDistribution(1, 15)) : 10;
	}

public:
	Line(int16_t width, int16_t height, int8_t length, bool epilepsy)
		: width_{ width }, height_{ height }, length_{ length }, epilepsy_{ epilepsy }
	{
		// тут x_ и y_ зависит от направления
		if constexpr (_Direction == Global::Direction::upToDown) {
			x_ = Global::getRandomUniformIntDistribution(static_cast<decltype(width_)>(0), width_);
			y_ = 0;
		}
		else if constexpr (_Direction == Global::Direction::downToUp) {
			x_ = Global::getRandomUniformIntDistribution(static_cast<decltype(width_)>(0), width_);
			y_ = height_;
		}
		else if constexpr (_Direction == Global::Direction::leftToRight) {
			x_ = 0;
			y_ = Global::getRandomUniformIntDistribution(static_cast<decltype(height_)>(0), height_);
		}
		else if constexpr (_Direction == Global::Direction::rightToLeft) {
			x_ = width_;
			y_ = Global::getRandomUniformIntDistribution(static_cast<decltype(height_)>(0), height_);
		}
		else throw std::logic_error{ "Класс Line инициализирован неизвестным типом!" };
	}

	// Добавляет ровно один символ в линию
	void addSymbol() {
		// Выбираем рандомный символ
		char symbol{ generateRandomSymbol() };

		// и задаём цвет
		int8_t color{ generateColor() };

		Symbols_.push_back(Symbol(xOffsetCounter_, yOffsetCounter_, symbol, color));
	}

	void move(double distance) {
		
		// Получаем текущую позицию начала линии
		int16_t x{ static_cast<int16_t>(x_) };
		int16_t y{ static_cast<int16_t>(y_) };
		// Накапливаем смещение
		if constexpr (_Direction == Global::Direction::upToDown) {
			y_ += distance;
		}
		else if constexpr (_Direction == Global::Direction::downToUp) {
			y_ -= distance;
		}
		else if constexpr (_Direction == Global::Direction::leftToRight) {
			x_ += distance;
		}
		else if constexpr (_Direction == Global::Direction::rightToLeft) {
			x_ -= distance;
		}
		
		// Вычисляем, на сколько позиций нам надо сместиться
		int16_t stepsY = static_cast<int16_t>(y_) - y; // вертикально
		int16_t stepsX = static_cast<int16_t>(x_) - x; // горизонтально


		// Если целочисленно нам пора бы уже сместиться
		while (stepsX != 0 || stepsY != 0) {
			// Если мы всё ещё не полностью добавили все символы линии
			if (Symbols_.size() < length_) {
				// Играемся с отступами как нам надо
				if constexpr (_Direction == Global::Direction::upToDown) {
					yOffsetCounter_++;
					// Добавляем символ
					addSymbol();
					y_--;

					stepsY--;
				}
				else if constexpr (_Direction == Global::Direction::downToUp) {
					yOffsetCounter_--;
					// Добавляем символ
					addSymbol();
					y_++;

					stepsY++;
				}
				else if constexpr (_Direction == Global::Direction::leftToRight) {
					xOffsetCounter_++;
					// Добавляем символ
					addSymbol();
					x_--;

					stepsX--;
				}
				else if constexpr (_Direction == Global::Direction::rightToLeft) {
					xOffsetCounter_--;
					// Добавляем символ
					addSymbol();
					x_++;

					stepsX++;
				}
				continue;
			}
			// Мы уже добавили всё, что только могли, теперь остаётся только перемещать уже существующие символы
		
			auto currentIt = Symbols_.begin();
			for (; y != static_cast<int16_t>(y_) || x != static_cast<int16_t>(x_);) {
				// Если позиция, где фактически должен быть стёрт символ, внутри экрана
				if (x == std::clamp(x, static_cast<int16_t>(0), static_cast<int16_t>(width_)) &&
					y == std::clamp(y, static_cast<int16_t>(0), static_cast<int16_t>(height_)))
					// Стираем символ с экрана в той точке, где его больше не будет
					currentIt->print(x, y, ' ');
				if constexpr (_Direction == Global::Direction::upToDown) {
					y++;
				}
				else if constexpr (_Direction == Global::Direction::downToUp) {
					y--;
				}
				else if constexpr (_Direction == Global::Direction::leftToRight) {
					x++;
				}
				else if constexpr (_Direction == Global::Direction::rightToLeft) {
					x--;
				}
			}
			for (; std::next(currentIt) != Symbols_.end(); currentIt++) {
				// Так же необходимо сместить в обратную сторону символы и их цвета
				currentIt->setColor(std::next(currentIt)->getColor());
				currentIt->setSymbol(std::next(currentIt)->getSymbol());
			}
			// сгенерировать последнему новый символ
			currentIt->setSymbol(generateRandomSymbol());
			currentIt->setColor(generateColor());
			if constexpr (_Direction == Global::Direction::upToDown) {
				stepsY--;
			}
			else if constexpr (_Direction == Global::Direction::downToUp) {
				stepsY++;
			}
			else if constexpr (_Direction == Global::Direction::leftToRight) {
				stepsX--;
			}
			else if constexpr (_Direction == Global::Direction::rightToLeft) {
				stepsX++;
			}
		}
	}

	int16_t getX() const {
		return static_cast<int16_t>(x_);
	}

	int16_t getY() const {
		return static_cast<int16_t>(y_);
	}

	// Печатает те символы, которые находятся внутри экрана
	void print(int16_t width, int16_t height) {
		// Обновляем данные о высоте и ширине экрана
		width_ = { width };
		height_ = { height };

		// Проходим по всем символам
		for (auto& Node : Symbols_) {
			// Узнаём позицию, где фактически должен быть отображён символ
			int16_t xSymbolPosition{ static_cast<int16_t>(x_ + Node.getXOffset()) };
			int16_t ySymbolPosition{ static_cast<int16_t>(y_ + Node.getYOffset()) };

			// Если эта позиция находится в пределах экрана
			if (xSymbolPosition == std::clamp(xSymbolPosition, static_cast<int16_t>(0), width_) &&
				ySymbolPosition == std::clamp(ySymbolPosition, static_cast<int16_t>(0), height_))
				// То печатаем символ
				Node.print(x_, y_);
			// Сбрасываем курсор
			Global::resetConsoleCursorPos<int>();
		}
	}
};