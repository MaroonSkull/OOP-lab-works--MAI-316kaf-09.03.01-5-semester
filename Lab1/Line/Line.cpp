#include <Line.hpp>

#include <random>



static std::random_device rd;
static std::ranlux24_base engine(rd());

char Line::generateSymbol(bool isSpace) {
	static std::uniform_int_distribution<int> Distribution(33, 126);
	return isSpace ? ' ' : Distribution(engine);
}

int Line::generateColor() {
	// цвет рандомный, если установлен флаг эпилепсии для символа, Цвет константно-рандомный, если установлен флаг эпилепсии для Линии. Ярко-зелёный (№10) по-умолчанию

	if constexpr (Global::myEpilepsyType == Global::EpilepsyType::symbol) {
		static std::uniform_int_distribution<int> Distribution(1, 15);
		return epilepsy ? Distribution(engine) : 10;
	}
	else if constexpr (Global::myEpilepsyType == Global::EpilepsyType::line)
		return epilepsy ? lineColor : 10;
}

bool Line::isCoordInsideFrame(int x, int y) const {
	return
		x == std::clamp(x, 0, width) &&
		y == std::clamp(y, 0, height);
}

Line::Line(int width, int height, int length, bool epilepsy)
	: width{ width }, height{ height }, length{ length }, epilepsy{ epilepsy }
{
	toggle = (width + height) % 2; // это позволяет нам выбирать, для зигзага сначала идёт левый или правый символ
	// а для ромба - идёт сначала один символ или сразу два.

	// Начальные значения для x и y зависят от направления варианта
	if constexpr (Global::myDirection == Global::Direction::upToDown) {
		y = 0;

		if constexpr (Global::myLineType == Global::LineType::line) {
			// линия занимает только один символ в ширину
			static std::uniform_int_distribution<int> Distribution(0, width);
			x = Distribution(engine);
		}
		else if constexpr (Global::myLineType == Global::LineType::zigzag) {
			// зиг-заг занимает свою клетку и клетку справа от себя
			static std::uniform_int_distribution<int> Distribution(0, width - 1);
			x = Distribution(engine);
		}
		else if constexpr (Global::myLineType == Global::LineType::rhombus) {
			// ромб занимает свою клетку и клетки по бокам от себя
			static std::uniform_int_distribution<int> Distribution(1, width - 1);
			x = Distribution(engine);
		}
	}
	else if constexpr (Global::myDirection == Global::Direction::downToUp) {
		y = height;

		if constexpr (Global::myLineType == Global::LineType::line) {
			// линия занимает только один символ в ширину
			static std::uniform_int_distribution<int> Distribution(0, width);
			x = Distribution(engine);
		}
		else if constexpr (Global::myLineType == Global::LineType::zigzag) {
			// зиг-заг занимает свою клетку и клетку справа от себя
			static std::uniform_int_distribution<int> Distribution(0, width - 1);
			x = Distribution(engine);
		}
		else if constexpr (Global::myLineType == Global::LineType::rhombus) {
			// ромб занимает свою клетку и клетки по бокам от себя
			static std::uniform_int_distribution<int> Distribution(1, width - 1);
			x = Distribution(engine);
		}
	}
	else if constexpr (Global::myDirection == Global::Direction::leftToRight) {
		x = 0;

		if constexpr (Global::myLineType == Global::LineType::line) {
			// линия занимает только один символ в ширину
			static std::uniform_int_distribution<int> Distribution(0, height);
			y = Distribution(engine);
		}
		else if constexpr (Global::myLineType == Global::LineType::zigzag) {
			// зиг-заг занимает свою клетку и клетку снизу от себя
			static std::uniform_int_distribution<int> Distribution(0, height - 1);
			y = Distribution(engine);
		}
		else if constexpr (Global::myLineType == Global::LineType::rhombus) {
			// ромб занимает свою клетку и клетки по бокам от себя
			static std::uniform_int_distribution<int> Distribution(1, height - 1);
			y = Distribution(engine);
		}
	}
	else if constexpr (Global::myDirection == Global::Direction::rightToLeft) {
		x = width;

		if constexpr (Global::myLineType == Global::LineType::line) {
			// линия занимает только один символ в ширину
			static std::uniform_int_distribution<int> Distribution(0, height);
			y = Distribution(engine);
		}
		else if constexpr (Global::myLineType == Global::LineType::zigzag) {
			// зиг-заг занимает свою клетку и клетку снизу от себя
			static std::uniform_int_distribution<int> Distribution(0, height - 1);
			y = Distribution(engine);
		}
		else if constexpr (Global::myLineType == Global::LineType::rhombus) {
			// ромб занимает свою клетку и клетки по бокам от себя
			static std::uniform_int_distribution<int> Distribution(1, height - 1);
			y = Distribution(engine);
		}
	}

	// Если нам надо генерировать цвет для всей линии сразу, то делаем это в конструкторе один раз
	if constexpr (Global::myEpilepsyType == Global::EpilepsyType::line) {
		static std::uniform_int_distribution<int> Distribution(1, 15);
		lineColor = Distribution(engine);
	}
}

void Line::addSymbol(bool isSpace) {
	// Если это не пробел, то выбираем рандомный символ
	char symbol = generateSymbol(isSpace);

	// и задаём цвет
	int color = generateColor();

	Symbols.push_back(Symbol(xOffsetCounter, yOffsetCounter, symbol, color));
}

void Line::move(double distance) {

	// Получаем текущую позицию начала линии и отбрасываем дробную часть
	int x = static_cast<int>(this->x);
	int y = static_cast<int>(this->y);

	// Накапливаем смещение
	if constexpr (Global::myDirection == Global::Direction::upToDown)
		this->y += distance;
	else if constexpr (Global::myDirection == Global::Direction::downToUp)
		this->y -= distance;
	else if constexpr (Global::myDirection == Global::Direction::leftToRight)
		this->x += distance;
	else if constexpr (Global::myDirection == Global::Direction::rightToLeft)
		this->x -= distance;

	// Вычисляем, на сколько позиций нам надо сместиться
	int stepsY = static_cast<int>(this->y) - y; // вертикально
	int stepsX = static_cast<int>(this->x) - x; // горизонтально


	// Если целочисленно нам пора бы уже сместиться
	while (stepsX != 0 || stepsY != 0) {

		// Если мы всё ещё не полностью добавили все символы линии
		bool symbolsCondition{}; // переменная нужна только с использованием if constexpr, лучше удалить и перенести условие в обычный if пониже
		if constexpr (Global::myLineType == Global::LineType::line)
			symbolsCondition = Symbols.size() < length;
		else if constexpr (Global::myLineType == Global::LineType::zigzag)
			symbolsCondition = Symbols.size() < length * 2;
		else if constexpr (Global::myLineType == Global::LineType::rhombus)
			symbolsCondition = Symbols.size() < length * 3;

		if (symbolsCondition) {
			// Играемся с отступами как нам надо
			if constexpr (Global::myDirection == Global::Direction::upToDown) {
				yOffsetCounter++;

				if constexpr (Global::myLineType == Global::LineType::line) {
					// Добавляем символ
					addSymbol();
				}
				else if constexpr (Global::myLineType == Global::LineType::zigzag) {
					if (toggle) { // в основной линии символ
						addSymbol();
						xOffsetCounter++;
						addSymbol(true);
					}
					else {
						addSymbol(true);
						xOffsetCounter++;
						addSymbol();
					}
					xOffsetCounter--;
					toggle = !toggle;
				}
				else if constexpr (Global::myLineType == Global::LineType::rhombus) {
					if (toggle) { // символ и два пробела вокруг
						xOffsetCounter--;
						addSymbol(true);
						xOffsetCounter++;
						addSymbol();
						xOffsetCounter++;
						addSymbol(true);
					}
					else { // два символа вокруг пробела
						xOffsetCounter--;
						addSymbol();
						xOffsetCounter++;
						addSymbol(true);
						xOffsetCounter++;
						addSymbol();
					}
					xOffsetCounter--;
					toggle = !toggle;
				}

				this->y--;

				stepsY--;
			}
			else if constexpr (Global::myDirection == Global::Direction::downToUp) {
				yOffsetCounter--;

				if constexpr (Global::myLineType == Global::LineType::line) {
					// Добавляем символ
					addSymbol();
				}
				else if constexpr (Global::myLineType == Global::LineType::zigzag) {
					if (toggle) { // в основной линии символ
						addSymbol();
						xOffsetCounter++;
						addSymbol(true);
					}
					else {
						addSymbol(true);
						xOffsetCounter++;
						addSymbol();
					}
					xOffsetCounter--;
					toggle = !toggle;
				}
				else if constexpr (Global::myLineType == Global::LineType::rhombus) {
					if (toggle) { // символ и два пробела вокруг
						xOffsetCounter--;
						addSymbol(true);
						xOffsetCounter++;
						addSymbol();
						xOffsetCounter++;
						addSymbol(true);
					}
					else { // два символа вокруг пробела
						xOffsetCounter--;
						addSymbol();
						xOffsetCounter++;
						addSymbol(true);
						xOffsetCounter++;
						addSymbol();
					}
					xOffsetCounter--;
					toggle = !toggle;
				}

				this->y++;

				stepsY++;
			}
			else if constexpr (Global::myDirection == Global::Direction::leftToRight) {
				xOffsetCounter++;

				if constexpr (Global::myLineType == Global::LineType::line) {
					// Добавляем символ
					addSymbol();
				}
				else if constexpr (Global::myLineType == Global::LineType::zigzag) {
					if (toggle) { // в основной линии символ
						addSymbol();
						yOffsetCounter++;
						addSymbol(true);
					}
					else {
						addSymbol(true);
						yOffsetCounter++;
						addSymbol();
					}
					yOffsetCounter--;
					toggle = !toggle;
				}
				else if constexpr (Global::myLineType == Global::LineType::rhombus) {
					if (toggle) { // символ и два пробела вокруг
						yOffsetCounter--;
						addSymbol(true);
						yOffsetCounter++;
						addSymbol();
						yOffsetCounter++;
						addSymbol(true);
					}
					else { // два символа вокруг пробела
						yOffsetCounter--;
						addSymbol();
						yOffsetCounter++;
						addSymbol(true);
						yOffsetCounter++;
						addSymbol();
					}
					yOffsetCounter--;
					toggle = !toggle;
				}

				this->x--;

				stepsX--;
			}
			else if constexpr (Global::myDirection == Global::Direction::rightToLeft) {
				xOffsetCounter--;

				if constexpr (Global::myLineType == Global::LineType::line) {
					// Добавляем символ
					addSymbol();
				}
				else if constexpr (Global::myLineType == Global::LineType::zigzag) {
					if (toggle) { // в основной линии символ
						addSymbol();
						yOffsetCounter++;
						addSymbol(true);
					}
					else {
						addSymbol(true);
						yOffsetCounter++;
						addSymbol();
					}
					yOffsetCounter--;
					toggle = !toggle;
				}
				else if constexpr (Global::myLineType == Global::LineType::rhombus) {
					if (toggle) { // символ и два пробела вокруг
						yOffsetCounter--;
						addSymbol(true);
						yOffsetCounter++;
						addSymbol();
						yOffsetCounter++;
						addSymbol(true);
					}
					else { // два символа вокруг пробела
						yOffsetCounter--;
						addSymbol();
						yOffsetCounter++;
						addSymbol(true);
						yOffsetCounter++;
						addSymbol();
					}
					yOffsetCounter--;
					toggle = !toggle;
				}

				this->x++;

				stepsX++;
			}
			continue;
		}
		// Мы уже добавили всё, что только могли, теперь остаётся только перемещать уже существующие символы
		auto currentIt = Symbols.begin();
		// Пытаемся стереть символы только если отключена полная очистка экрана
		if constexpr (!Global::enableClearScreen)
			for (; y != static_cast<int>(this->y) || x != static_cast<int>(this->x);) {
				// Если позиция, где начинается линия внутри экрана, то все символы первой строки линии точно будут внутри экрана
				if (isCoordInsideFrame(x, y)) {
					if constexpr (Global::myLineType == Global::LineType::line)
						// Стираем символ с экрана в той точке, где его больше не будет
						currentIt->print(x, y, ' ');
					else if constexpr (Global::myLineType == Global::LineType::zigzag) {
						// стираем оба символа, что должны быть в начале (нас не волнует стирание пробельных символов)
						currentIt->print(x, y, ' ');
						currentIt++;
						int xSymb = x + currentIt->getYOffset();
						int ySymb = y + currentIt->getYOffset();
						currentIt->print(xSymb, ySymb, ' ');
						currentIt--;
					}
					else if constexpr (Global::myLineType == Global::LineType::rhombus) {
						int xSymb = x + currentIt->getXOffset();
						int ySymb = y + currentIt->getYOffset();
						// стираем три начальных символа
						currentIt->print(xSymb, ySymb, ' ');
						currentIt++;
						xSymb = x + currentIt->getXOffset();
						ySymb = y + currentIt->getYOffset();
						currentIt->print(xSymb, ySymb, ' ');
						currentIt++;
						xSymb = x + currentIt->getXOffset();
						ySymb = y + currentIt->getYOffset();
						currentIt->print(xSymb, ySymb, ' ');
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
			if (Symbols.size() >= 1) {
				for (; currentIt != std::prev(Symbols.end()); currentIt++) {
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
			if (Symbols.size() >= 2) {
				for (; currentIt != std::prev(Symbols.end(), 2); currentIt++) {
					// копируем на два символа вперёд
					currentIt->setColor(std::next(currentIt, 2)->getColor());
					currentIt->setSymbol(std::next(currentIt, 2)->getSymbol());
				}
				// сгенерировать последним двум новый символ
				if (toggle) { // в основной линии символ
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
				toggle = !toggle;
			}
		}
		else if constexpr (Global::myLineType == Global::LineType::rhombus) {
			if (Symbols.size() >= 3) {
				// копируем через 3 символа вперёд
				for (; currentIt != std::prev(Symbols.end(), 3); currentIt++) {
					currentIt->setColor(std::next(currentIt, 3)->getColor());
					currentIt->setSymbol(std::next(currentIt, 3)->getSymbol());
				}
				// сгенерировать последним трём новый символ
				if (toggle) { // символ и два пробела вокруг
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
				toggle = !toggle;
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

void Line::print(int width, int height) {
	// Обновляем данные о высоте и ширине экрана
	this->width = width;
	this->height = height;

	// Проходим по всем символам
	for (auto &Symbol : Symbols) {
		// Округляем позицию начала линии до целых
		int xHeadPosition{ static_cast<int>(x) };
		int yHeadPosition{ static_cast<int>(y) };

		// Узнаём позицию, где фактически должен быть отображён символ
		int xSymbolPosition{ xHeadPosition + Symbol.getXOffset() };
		int ySymbolPosition{ yHeadPosition + Symbol.getYOffset() };

		// Если эти позиции находятся в пределах экрана
		if (isCoordInsideFrame(xSymbolPosition, ySymbolPosition) && isCoordInsideFrame(xHeadPosition, yHeadPosition))
			// То печатаем символ
			Symbol.print(xSymbolPosition, ySymbolPosition);

		// Сбрасываем курсор
		//Global::setConsoleCursorPos(0, 0);
	}
}
