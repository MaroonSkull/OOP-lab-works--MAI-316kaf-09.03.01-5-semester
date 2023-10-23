#include <Symbol.hpp>

void Symbol::print(int16_t x, int16_t y) {
	print(x, y, symbol_);
}

void Symbol::print(int16_t x, int16_t y, char symbol) {
	Global::setConsoleCursorPos(x, y);
	Global::setConsoleColor(color_);
	std::cout << symbol;
}