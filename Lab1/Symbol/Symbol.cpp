#include <Symbol.hpp>

void Symbol::print(int16_t baseX, int16_t baseY) {
	print(baseX, baseY, symbol_);
}

void Symbol::print(int16_t baseX, int16_t baseY, char symbol) {
	int16_t consoleX{ static_cast<int16_t>(baseX) + offsetX_ };
	int16_t consoleY{ static_cast<int16_t>(baseY) + offsetY_ };

	Global::setConsoleCursorPos(consoleX, consoleY);
	Global::setConsoleColor(color_);
	std::cout << symbol;
}