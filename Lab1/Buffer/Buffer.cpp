#include <Buffer.hpp>
#include <Global.hpp>

void Buffer::resize(int16_t width, int16_t height) {
	destinationRect_.Right = width;
	destinationRect_.Bottom = height;

	buffer_.resize(height);
	for (auto& row : buffer_)
		row.resize(width);

	clear();
}

void Buffer::clear() {
	for (auto &row : buffer_)
		for (auto& symbol : row) {
			symbol.Char.AsciiChar = ' ';
			symbol.Attributes = static_cast<WORD>(15);
		}
}

void Buffer::print() {
	WriteConsoleOutput(
		Global::hConsole,
		&buffer_.at(0).at(0),
		COORD {
			.X = static_cast<int16_t>(buffer_.size()),
			.Y = static_cast<int16_t>(buffer_.at(0).size())
		},
		Global::tl,
		&destinationRect_
	);
}
