#include <Symbol.hpp>

#include <iostream>



const static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


void Symbol::setConsoleCursorPos(int x, int y) const {
	COORD pos(x, y);
	SetConsoleCursorPosition(hConsole, pos);
}

void Symbol::setConsoleColor(int color) const {
	WORD wColor = static_cast<WORD>(color & 0x0F);
	SetConsoleTextAttribute(hConsole, wColor);
}

void Symbol::print(int x, int y) {
	print(x, y, symbol);
}

void Symbol::print(int x, int y, char symbol) {
	setConsoleCursorPos(x, y);
	setConsoleColor(color);
	std::cout << symbol;
}