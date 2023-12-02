#include <Symbol.hpp>

void Symbol::print(Buffer& Buff, int16_t x, int16_t y) {
	print(Buff, x, y, symbol_);
}

void Symbol::print(Buffer& Buff, int16_t x, int16_t y, char symbol) {
	auto& [sym, color] = Buff.getChar(x, y);
	
	sym = symbol;
	color = COLOR_PAIR(color_ % 8);
	if (color_ >= 8) color |= A_BOLD;
}