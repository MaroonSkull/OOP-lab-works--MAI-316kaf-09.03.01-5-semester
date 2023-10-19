#pragma once
#include <random>

namespace Global {

	constexpr auto maxFPS = 60.0;

	enum class Direction {
		leftToRight,
		upToDown,
		rightToLeft,
		downToUp
	};

	static const Direction myDirection = Direction::rightToLeft;

	const static COORD tl = { 0,0 };
	const static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//const static HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	template <typename T>
	void setConsoleColor(T ForgC) {
		WORD wColor = ForgC & 0x0F;
		SetConsoleTextAttribute(hConsole, wColor);
	}

	template <typename T>
	void setConsoleCursorPos(T x, T y) {
		COORD pos = { x, y };
		SetConsoleCursorPosition(hConsole, pos);
	}

	template <typename T>
	void resetConsoleCursorPos() {
		SetConsoleCursorPosition(hConsole, tl);
	}

	template <typename T>
	T getRandomUniformIntDistribution(T min, T max) {
		static std::random_device rd_;
		static std::ranlux24_base engine_(rd_());
		static std::uniform_int_distribution<T> uniformDist_(min, max);
		return uniformDist_(engine_);
	}
}