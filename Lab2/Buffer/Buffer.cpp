#include <Buffer.hpp>
#include <Global.hpp>
#include <ncurses.h>

void Buffer::resize(int16_t width, int16_t height) {
	x_ = width + 1;
	y_ = height + 1;

	buffer_.resize(x_ * y_);
	clear();
}

void Buffer::clear() {
	for (auto& [symbol, color] : buffer_) {
		symbol = ' '; // пробельный символ
		color = COLOR_PAIR(COLOR_GREEN) | A_BOLD; // по-умолчанию ярко-зелёный цвет
	}
}

void Buffer::print() {
	/*if (!WriteConsoleOutput(Global::hConsole, &buffer_.at(0), Sizes_, Global::tl, &destinationRect_)) {
		std::stringstream ss;
		ss << "WriteConsoleOutput failed - (" << GetLastError() << ")";
		throw std::runtime_error{ ss.str() };
	}*/

	
	move(0, 0);
	for (auto &[symbol, color] : buffer_) {
		attron(color);
		printw("%c", symbol);
		attroff(color);
	}
	refresh();
}

Buffer::CHAR_INFO& Buffer::getChar(int16_t consoleX, int16_t consoleY) {
	using namespace std::string_literals;
	if (consoleY * x_ + consoleX > buffer_.size())
		throw std::out_of_range{ "consoleX = "s + std::to_string(consoleX) + ", consoleY = " + std::to_string(consoleY) + ". x_ = " + std::to_string(x_) + ", y_ = " + std::to_string(y_) + ". buffer_.size() = " + std::to_string(buffer_.size()) };
	return buffer_.at(consoleY * x_ + consoleX);
}