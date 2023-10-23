#include <Buffer.hpp>
#include <Global.hpp>
#include <sstream>

void Buffer::resize(int16_t width, int16_t height) {
	destinationRect_.Right = width;
	destinationRect_.Bottom = height;
	
	Sizes_ = {
		.X = static_cast<int16_t>(width + 1),
		.Y = static_cast<int16_t>(height + 1)
	};

	buffer_.resize(Sizes_.X * Sizes_.Y);

	clear();
}

void Buffer::clear() {
	for (auto& symbol : buffer_) {
		symbol.Char.AsciiChar = ' ';
		symbol.Attributes = static_cast<WORD>(15);
	}
}

void Buffer::print() {
	if (!WriteConsoleOutput(Global::hConsole, &buffer_.at(0), Sizes_, Global::tl, &destinationRect_)) {
		std::stringstream ss;
		ss << "WriteConsoleOutput failed - (" << GetLastError() << ")";
		throw std::runtime_error{ ss.str() };
	}
}

CHAR_INFO& Buffer::getChar(int16_t consoleX, int16_t consoleY) {
	using namespace std::string_literals;
	if (consoleY * Sizes_.X + consoleX > buffer_.size())
		throw std::out_of_range{ "consoleX = "s + std::to_string(consoleX) + ", consoleY = " + std::to_string(consoleY) + ". Sizes_.X = " + std::to_string(Sizes_.X) + ", Sizes_.Y = " + std::to_string(Sizes_.Y) + ". buffer_.size() = " + std::to_string(buffer_.size()) };
	return buffer_.at(consoleY * Sizes_.X + consoleX);
}