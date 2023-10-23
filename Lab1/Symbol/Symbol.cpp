#include <Symbol.hpp>

void Symbol::print(Buffer& Buff, int16_t x, int16_t y) {
	print(Buff, x, y, symbol_);
}

void Symbol::print(Buffer& Buff, int16_t x, int16_t y, char symbol) {
	auto& [sym, color] = Buff.getChar(x, y);
	
	sym.AsciiChar = symbol;
	color = static_cast<WORD>(color_ & 0x0F);
}