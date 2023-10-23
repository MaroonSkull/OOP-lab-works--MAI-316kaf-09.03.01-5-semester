#include <Line.hpp>

char Line::generateSymbol(bool isSpace) {
	return isSpace ? ' ' : static_cast<char>(Global::getRandomUniformIntDistribution(33, 126));;
}

int8_t Line::generateColor() {
	// цвет рандомный, если установлен флаг эпилепсии для символа, Цвет константно-рандомный, если установлен флаг эпилепсии для Линии. Ярко-зелёный (№10) по-умолчанию

	if constexpr (Global::myEpilepsyType == Global::EpilepsyType::symbol)
		return epilepsy_ ? static_cast<int8_t>(Global::getRandomUniformIntDistribution(1, 15)) : 10;
	else if constexpr (Global::myEpilepsyType == Global::EpilepsyType::line)
		return epilepsy_ ? lineColor_ : 10;
}

Line::Line(int16_t width, int16_t height, int8_t length, bool epilepsy)
	: width_{ width }, height_{ height }, length_{ length }, epilepsy_{ epilepsy }
{
	toggle_ = (width_ + height_) % 2; // это позволяет нам выбирать, для зигзага сначала идёт левый или правый символ
	// а для ромба - идёт сначала один символ или сразу два.

	// TODO: перенести это декартово произведение на std::variant + std::visit, правда речь про RTTI, а не про compile-time вычисления
	// тут x_ и y_ зависит от направления
	if constexpr (Global::myDirection == Global::Direction::upToDown) {
		y_ = 0;

		if constexpr (Global::myLineType == Global::LineType::line)
			// линия занимает только один символ в ширину
			x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), width_);
		else if constexpr (Global::myLineType == Global::LineType::zigzag)
			// зиг-заг занимает свою клетку и клетку справа от себя
			x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), static_cast<int16_t>(width_ - 1));
		else if constexpr (Global::myLineType == Global::LineType::rhombus)
			// ромб занимает свою клетку и клетки по бокам от себя
			x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(1), static_cast<int16_t>(width_ - 1));
	}
	else if constexpr (Global::myDirection == Global::Direction::downToUp) {
		y_ = height_;

		if constexpr (Global::myLineType == Global::LineType::line)
			// линия занимает только один символ в ширину
			x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), width_);
		else if constexpr (Global::myLineType == Global::LineType::zigzag)
			// зиг-заг занимает свою клетку и клетку справа от себя
			x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), static_cast<int16_t>(width_ - 1));
		else if constexpr (Global::myLineType == Global::LineType::rhombus)
			// ромб занимает свою клетку и клетки по бокам от себя
			x_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(1), static_cast<int16_t>(width_ - 1));
	}
	else if constexpr (Global::myDirection == Global::Direction::leftToRight) {
		x_ = 0;

		if constexpr (Global::myLineType == Global::LineType::line)
			// линия занимает только один символ в ширину
			y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), height_);
		else if constexpr (Global::myLineType == Global::LineType::zigzag)
			// зиг-заг занимает свою клетку и клетку снизу от себя
			y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), static_cast<int16_t>(height_ - 1));
		else if constexpr (Global::myLineType == Global::LineType::rhombus)
			// ромб занимает свою клетку и клетки по бокам от себя
			y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(1), static_cast<int16_t>(height_ - 1));
	}
	else if constexpr (Global::myDirection == Global::Direction::rightToLeft) {
		x_ = width_;

		if constexpr (Global::myLineType == Global::LineType::line)
			// линия занимает только один символ в ширину
			y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), height_);
		else if constexpr (Global::myLineType == Global::LineType::zigzag)
			// зиг-заг занимает свою клетку и клетку снизу от себя
			y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(0), static_cast<int16_t>(height_ - 1));
		else if constexpr (Global::myLineType == Global::LineType::rhombus)
			// ромб занимает свою клетку и клетки по бокам от себя
			y_ = Global::getRandomUniformIntDistribution(static_cast<int16_t>(1), static_cast<int16_t>(height_ - 1));
	}

	// Если нам надо генерировать цвет для всей линии сразу, то делаем это в конструкторе один раз
	if constexpr (Global::myEpilepsyType == Global::EpilepsyType::line)
		lineColor_ = static_cast<int8_t>(Global::getRandomUniformIntDistribution(1, 15));
}

void Line::addSymbol(bool isSpace) {
	// Если это не пробел, то выбираем рандомный символ
	char symbol{ generateSymbol(isSpace) };

	// и задаём цвет
	int8_t color{ generateColor() };

	Symbols_.push_back(Symbol(xOffsetCounter_, yOffsetCounter_, symbol, color));
}

void Line::move(Buffer &Buff, double distance) {

	// Получаем текущую позицию начала линии
	int16_t x{ static_cast<int16_t>(x_) };
	int16_t y{ static_cast<int16_t>(y_) };
	// Накапливаем смещение
	if constexpr (Global::myDirection == Global::Direction::upToDown)
		y_ += distance;
	else if constexpr (Global::myDirection == Global::Direction::downToUp)
		y_ -= distance;
	else if constexpr (Global::myDirection == Global::Direction::leftToRight)
		x_ += distance;
	else if constexpr (Global::myDirection == Global::Direction::rightToLeft)
		x_ -= distance;

	// Вычисляем, на сколько позиций нам надо сместиться
	int16_t stepsY = static_cast<int16_t>(y_) - y; // вертикально
	int16_t stepsX = static_cast<int16_t>(x_) - x; // горизонтально


	// Если целочисленно нам пора бы уже сместиться
	while (stepsX != 0 || stepsY != 0) {

		// Если мы всё ещё не полностью добавили все символы линии
		bool symbolsCondition{}; // переменная нужна только с использованием if constexpr, лучше удалить и перенести условие в обычный if пониже
		if constexpr (Global::myLineType == Global::LineType::line)
			symbolsCondition = Symbols_.size() < length_;
		else if constexpr (Global::myLineType == Global::LineType::zigzag)
			symbolsCondition = Symbols_.size() < length_ * 2;
		else if constexpr (Global::myLineType == Global::LineType::rhombus)
			symbolsCondition = Symbols_.size() < length_ * 3;

		if (symbolsCondition) {
			// Играемся с отступами как нам надо
			if constexpr (Global::myDirection == Global::Direction::upToDown) {
				yOffsetCounter_++;

				if constexpr (Global::myLineType == Global::LineType::line) {
					// Добавляем символ
					addSymbol();
				}
				else if constexpr (Global::myLineType == Global::LineType::zigzag) {
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
				else if constexpr (Global::myLineType == Global::LineType::rhombus) {
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
			else if constexpr (Global::myDirection == Global::Direction::downToUp) {
				yOffsetCounter_--;

				if constexpr (Global::myLineType == Global::LineType::line) {
					// Добавляем символ
					addSymbol();
				}
				else if constexpr (Global::myLineType == Global::LineType::zigzag) {
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
				else if constexpr (Global::myLineType == Global::LineType::rhombus) {
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
			else if constexpr (Global::myDirection == Global::Direction::leftToRight) {
				xOffsetCounter_++;

				if constexpr (Global::myLineType == Global::LineType::line) {
					// Добавляем символ
					addSymbol();
				}
				else if constexpr (Global::myLineType == Global::LineType::zigzag) {
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
				else if constexpr (Global::myLineType == Global::LineType::rhombus) {
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
			else if constexpr (Global::myDirection == Global::Direction::rightToLeft) {
				xOffsetCounter_--;

				if constexpr (Global::myLineType == Global::LineType::line) {
					// Добавляем символ
					addSymbol();
				}
				else if constexpr (Global::myLineType == Global::LineType::zigzag) {
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
				else if constexpr (Global::myLineType == Global::LineType::rhombus) {
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
				if constexpr (Global::myLineType == Global::LineType::line)
					// Стираем символ с экрана в той точке, где его больше не будет
					currentIt->print(Buff, x, y, ' ');
				else if constexpr (Global::myLineType == Global::LineType::zigzag) {
					// стираем оба символа, что должны быть в начале (нас не волнует стирание пробельных символов)
					currentIt->print(Buff, x, y, ' ');
					currentIt++;
					currentIt->print(Buff, x, y, ' ');
					currentIt--;
				}
				else if constexpr (Global::myLineType == Global::LineType::rhombus) {
					// стираем три начальных символа
					currentIt->print(Buff, x, y, ' ');
					currentIt++;
					currentIt->print(Buff, x, y, ' ');
					currentIt++;
					currentIt->print(Buff, x, y, ' ');
					currentIt--;
					currentIt--;
				}
			}
			if constexpr (Global::myDirection == Global::Direction::upToDown) {
				y++;
			}
			else if constexpr (Global::myDirection == Global::Direction::downToUp) {
				y--;
			}
			else if constexpr (Global::myDirection == Global::Direction::leftToRight) {
				x++;
			}
			else if constexpr (Global::myDirection == Global::Direction::rightToLeft) {
				x--;
			}
		}
		// Так же необходимо сместить в обратную сторону символы и их цвета
		if constexpr (Global::myLineType == Global::LineType::line) {
			if (Symbols_.size() >= 1) {
				for (; std::next(currentIt) != Symbols_.end(); currentIt++) {
					// копируем только один символ
					currentIt->setColor(std::next(currentIt)->getColor());
					currentIt->setSymbol(std::next(currentIt)->getSymbol());
				}
				// сгенерировать последнему новый символ
				currentIt->setSymbol(generateSymbol());
				currentIt->setColor(generateColor());
			}
		}
		else if constexpr (Global::myLineType == Global::LineType::zigzag) {
			if (Symbols_.size() >= 2) {
				for (; std::next(std::next(currentIt)) != Symbols_.end(); currentIt++) {
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
					currentIt--;
				}
				else {
					currentIt->setSymbol(generateSymbol(true));
					currentIt->setColor(generateColor());
					currentIt++;
					currentIt->setSymbol(generateSymbol());
					currentIt->setColor(generateColor());
					currentIt--;
				}
				toggle_ = !toggle_;
			}
		}
		else if constexpr (Global::myLineType == Global::LineType::rhombus) {
			if (Symbols_.size() >= 3) {
				// копируем через 3 символа вперёд
				for (; std::next(std::next(std::next(currentIt))) != Symbols_.end(); currentIt++) {
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
		}

		if constexpr (Global::myDirection == Global::Direction::upToDown) {
			stepsY--;
		}
		else if constexpr (Global::myDirection == Global::Direction::downToUp) {
			stepsY++;
		}
		else if constexpr (Global::myDirection == Global::Direction::leftToRight) {
			stepsX--;
		}
		else if constexpr (Global::myDirection == Global::Direction::rightToLeft) {
			stepsX++;
		}
	}
}

void Line::print(Buffer& Buff, int16_t width, int16_t height) {
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
			Node.print(Buff, static_cast<int16_t>(x_), static_cast<int16_t>(y_));
		// Сбрасываем курсор
		Global::setConsoleCursorPos(0, 0);
	}
}
