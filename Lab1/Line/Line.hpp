#pragma once

#include <Symbol.hpp>
#include <Global.hpp>

#include <iostream>
#include <list>
#include <functional>


template <Global::Direction _Direction, Global::LineType _LineType> 
class Line {
private:
	std::list<Symbol> Symbols_;
	// базовые координаты, от которых начинается линия
	double x_{};
	double y_{};

	int8_t xOffsetCounter_{};
	int8_t yOffsetCounter_{};

	bool toggle_{};

	int16_t width_;
	int16_t height_;
	int8_t length_{};
	bool epilepsy_;


	char generateSymbol(bool isSpace = false) {
		return isSpace ? ' ' : static_cast<char>(Global::getRandomUniformIntDistribution(33, 126));
	}

	int8_t generateColor() {
		// цвет рандомный, если установлен флаг эпилепсии, ярко-зелёный (№10) по-умолчанию
		return epilepsy_ ? static_cast<int8_t>(Global::getRandomUniformIntDistribution(1, 15)) : 10;
	}

public:
	Line(int16_t width, int16_t height, int8_t length, bool epilepsy)
		: width_{ width }, height_{ height }, length_{ length }, epilepsy_{ epilepsy }
	{
		toggle_ = (width_ + height_) % 2; // это позволяет нам выбирать, для зигзага сначала идёт левый или правый символ
		// а для ромба - идёт сначала один символ или сразу два.

		// TODO: перенести это декартово произведение на std::variant + std::visit, правда речь про RTTI, а не про compile-time вычисления
		// тут x_ и y_ зависит от направления
		if constexpr (_Direction == Global::Direction::upToDown) {
			y_ = 0;
			if constexpr (_LineType == Global::LineType::line)
				// линия занимает только один символ в ширину
				x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), width_);
			else if constexpr (_LineType == Global::LineType::zigzag)
				// зиг-заг занимает свою клетку и клетку справа от себя
				x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), static_cast<int16_t>(width_ - 1));
			else if constexpr (_LineType == Global::LineType::rhombus)
				// ромб занимает свою клетку и клетки по бокам от себя
				x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(1), static_cast<int16_t>(width_ - 1));
		}
		else if constexpr (_Direction == Global::Direction::downToUp) {
			y_ = height_;
			if constexpr (_LineType == Global::LineType::line)
				// линия занимает только один символ в ширину
				x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), width_);
			else if constexpr (_LineType == Global::LineType::zigzag)
				// зиг-заг занимает свою клетку и клетку справа от себя
				x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), static_cast<int16_t>(width_ - 1));
			else if constexpr (_LineType == Global::LineType::rhombus)
				// ромб занимает свою клетку и клетки по бокам от себя
				x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(1), static_cast<int16_t>(width_ - 1));
		}
		else if constexpr (_Direction == Global::Direction::leftToRight) {
			x_ = 0;
			if constexpr (_LineType == Global::LineType::line)
				// линия занимает только один символ в ширину
				y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), height_);
			else if constexpr (_LineType == Global::LineType::zigzag)
				// зиг-заг занимает свою клетку и клетку снизу от себя
				y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), static_cast<int16_t>(height_-1));
			else if constexpr (_LineType == Global::LineType::rhombus)
				// ромб занимает свою клетку и клетки по бокам от себя
				y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(1), static_cast<int16_t>(height_ - 1));
		}
		else if constexpr (_Direction == Global::Direction::rightToLeft) {
			x_ = width_;
			if constexpr (_LineType == Global::LineType::line)
				// линия занимает только один символ в ширину
				y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), height_);
			else if constexpr (_LineType == Global::LineType::zigzag)
				// зиг-заг занимает свою клетку и клетку снизу от себя
				y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), static_cast<int16_t>(height_ - 1));
			else if constexpr (_LineType == Global::LineType::rhombus)
					// ромб занимает свою клетку и клетки по бокам от себя
					y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(1), static_cast<int16_t>(height_ - 1));
		}
		else throw std::logic_error{ "Класс Line инициализирован неизвестным типом!" };
	}

	// todo подумать о том, куда перенести тернарные операторы
	// Добавляет ровно один символ в линию
	void addSymbol(bool isSpace = false) {
		// Если это не пробел, то выбираем рандомный символ
		char symbol{ generateSymbol(isSpace) };

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
					
					if constexpr (_LineType == Global::LineType::line) {
						// Добавляем символ
						addSymbol();
					}
					else if constexpr (_LineType == Global::LineType::zigzag) {
						if (toggle_) { // в основной линии символ
							addSymbol();
							xOffsetCounter_++;
							addSymbol(true);
						}
						else {
							addSymbol(true);
							xOffsetCounter_++;
							addSymbol();
						}
						xOffsetCounter_--;
						toggle_ = !toggle_;
					}
					else if constexpr (_LineType == Global::LineType::rhombus) {
						if (toggle_) { // символ и два пробела вокруг
							xOffsetCounter_--;
							addSymbol(true);
							xOffsetCounter_++;
							addSymbol();
							xOffsetCounter_++;
							addSymbol(true);
						}
						else { // два символа вокруг пробела
							xOffsetCounter_--;
							addSymbol();
							xOffsetCounter_++;
							addSymbol(true);
							xOffsetCounter_++;
							addSymbol();
						}
						xOffsetCounter_--;
						toggle_ = !toggle_;
					}

					y_--;

					stepsY--;
				}
				else if constexpr (_Direction == Global::Direction::downToUp) {
					yOffsetCounter_--;

					if constexpr (_LineType == Global::LineType::line) {
						// Добавляем символ
						addSymbol();
					}
					else if constexpr (_LineType == Global::LineType::zigzag) {
						if (toggle_) { // в основной линии символ
							addSymbol();
							xOffsetCounter_++;
							addSymbol(true);
						}
						else {
							addSymbol(true);
							xOffsetCounter_++;
							addSymbol();
						}
						xOffsetCounter_--;
						toggle_ = !toggle_;
					}
					else if constexpr (_LineType == Global::LineType::rhombus) {
						if (toggle_) { // символ и два пробела вокруг
							xOffsetCounter_--;
							addSymbol(true);
							xOffsetCounter_++;
							addSymbol();
							xOffsetCounter_++;
							addSymbol(true);
						}
						else { // два символа вокруг пробела
							xOffsetCounter_--;
							addSymbol();
							xOffsetCounter_++;
							addSymbol(true);
							xOffsetCounter_++;
							addSymbol();
						}
						xOffsetCounter_--;
						toggle_ = !toggle_;
					}

					y_++;

					stepsY++;
				}
				else if constexpr (_Direction == Global::Direction::leftToRight) {
					xOffsetCounter_++;
					
					if constexpr (_LineType == Global::LineType::line) {
						// Добавляем символ
						addSymbol();
					}
					else if constexpr (_LineType == Global::LineType::zigzag) {
						if (toggle_) { // в основной линии символ
							addSymbol();
							yOffsetCounter_++;
							addSymbol(true);
						}
						else {
							addSymbol(true);
							yOffsetCounter_++;
							addSymbol();
						}
						yOffsetCounter_--;
						toggle_ = !toggle_;
					}
					else if constexpr (_LineType == Global::LineType::rhombus) {
						if (toggle_) { // символ и два пробела вокруг
							yOffsetCounter_--;
							addSymbol(true);
							yOffsetCounter_++;
							addSymbol();
							yOffsetCounter_++;
							addSymbol(true);
						}
						else { // два символа вокруг пробела
							yOffsetCounter_--;
							addSymbol();
							yOffsetCounter_++;
							addSymbol(true);
							yOffsetCounter_++;
							addSymbol();
						}
						yOffsetCounter_--;
						toggle_ = !toggle_;
					}

					x_--;

					stepsX--;
				}
				else if constexpr (_Direction == Global::Direction::rightToLeft) {
					xOffsetCounter_--;
					
					if constexpr (_LineType == Global::LineType::line) {
						// Добавляем символ
						addSymbol();
					}
					else if constexpr (_LineType == Global::LineType::zigzag) {
						if (toggle_) { // в основной линии символ
							addSymbol();
							yOffsetCounter_++;
							addSymbol(true);
						}
						else {
							addSymbol(true);
							yOffsetCounter_++;
							addSymbol();
						}
						yOffsetCounter_--;
						toggle_ = !toggle_;
					}
					else if constexpr (_LineType == Global::LineType::rhombus) {
						if (toggle_) { // символ и два пробела вокруг
							yOffsetCounter_--;
							addSymbol(true);
							yOffsetCounter_++;
							addSymbol();
							yOffsetCounter_++;
							addSymbol(true);
						}
						else { // два символа вокруг пробела
							yOffsetCounter_--;
							addSymbol();
							yOffsetCounter_++;
							addSymbol(true);
							yOffsetCounter_++;
							addSymbol();
						}
						yOffsetCounter_--;
						toggle_ = !toggle_;
					}

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
					y == std::clamp(y, static_cast<int16_t>(0), static_cast<int16_t>(height_))) {
					if constexpr (_LineType == Global::LineType::line)
						// Стираем символ с экрана в той точке, где его больше не будет
						currentIt->print(x, y, ' ');
					else if constexpr (_LineType == Global::LineType::zigzag) {
						// стираем оба символа, что должны быть в начале (нас не волнует стирание пробельных символов)
						currentIt->print(x, y, ' ');
						currentIt++;
						currentIt->print(x, y, ' ');
					}
					else if constexpr (_LineType == Global::LineType::rhombus) {
						// стираем три начальных символа
						currentIt->print(x, y, ' ');
						currentIt++;
						currentIt->print(x, y, ' ');
						currentIt++;
						currentIt->print(x, y, ' ');
					}
				}
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
			// Так же необходимо сместить в обратную сторону символы и их цвета
			if constexpr (_LineType == Global::LineType::line) {
				for (; std::next(currentIt) != Symbols_.end(); currentIt++) {
					// копируем только один символ
					currentIt->setColor(std::next(currentIt)->getColor());
					currentIt->setSymbol(std::next(currentIt)->getSymbol());
				}
				// сгенерировать последнему новый символ
				currentIt->setSymbol(generateSymbol());
				currentIt->setColor(generateColor());
			}
			else if constexpr (_LineType == Global::LineType::zigzag) {
				for (; std::next(std::next(currentIt)) != Symbols_.end() && Symbols_.size() >=2; currentIt++) {
					// копируем на два символа вперёд
					currentIt->setColor(std::next(std::next(currentIt))->getColor());
					currentIt->setSymbol(std::next(std::next(currentIt))->getSymbol());
				}
				// сгенерировать последним двум новый символ
				if (toggle_) { // в основной линии символ
					currentIt->setSymbol(generateSymbol());
					currentIt->setColor(generateColor());
					currentIt++;
					currentIt->setSymbol(generateSymbol(true));
					currentIt->setColor(generateColor());
				}
				else {
					currentIt->setSymbol(generateSymbol(true));
					currentIt->setColor(generateColor());
					currentIt++;
					currentIt->setSymbol(generateSymbol());
					currentIt->setColor(generateColor());
				}
				toggle_ = !toggle_;
			}
			else if constexpr (_LineType == Global::LineType::rhombus) {
				// копируем через 3 символа вперёд
				for (; std::next(std::next(std::next(currentIt))) != Symbols_.end() && Symbols_.size() >=3; currentIt++) {
					currentIt->setColor(std::next(std::next(std::next(currentIt)))->getColor());
					currentIt->setSymbol(std::next(std::next(std::next(currentIt)))->getSymbol());
				}
				// сгенерировать последним трём новый символ
				if (toggle_) { // символ и два пробела вокруг
					currentIt->setSymbol(generateSymbol(true));
					currentIt->setColor(generateColor());
					currentIt++;
					currentIt->setSymbol(generateSymbol());
					currentIt->setColor(generateColor());
					currentIt++;
					currentIt->setSymbol(generateSymbol(true));
					currentIt->setColor(generateColor());
				}
				else { // два символа вокруг пробела
					currentIt->setSymbol(generateSymbol());
					currentIt->setColor(generateColor());
					currentIt++;
					currentIt->setSymbol(generateSymbol(true));
					currentIt->setColor(generateColor());
					currentIt++;
					currentIt->setSymbol(generateSymbol());
					currentIt->setColor(generateColor());
				}
				toggle_ = !toggle_;
			}

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