#include <Symbol.hpp>

void Symbol::print(Buffer& Buff, int16_t baseX, int16_t baseY) {
	print(Buff, baseX, baseY, symbol_);
}

void Symbol::print(Buffer& Buff, int16_t baseX, int16_t baseY, char symbol) {
	int16_t consoleX{ static_cast<int16_t>(baseX) + offsetX_ };
	int16_t consoleY{ static_cast<int16_t>(baseY) + offsetY_ };

	auto& sym{ Buff.getBuffer().at(consoleX).at(consoleY) };
	
	sym.Attributes = static_cast<WORD>(color_ & 0x0F);
	sym.Char.AsciiChar = symbol;
}