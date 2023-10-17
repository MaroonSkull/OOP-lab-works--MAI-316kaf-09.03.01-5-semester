#pragma once
#include <random>

namespace Global {

	const static COORD tl = { 0,0 };
	const static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	template <typename T>
	void setConsoleColor(T ForgC) {
		WORD wColor = ForgC & 0x0F;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
	}

	template <typename T>
	T getRandomUniformIntDistribution(T min, T max) {
		static std::random_device rd_;
		static std::ranlux24_base engine_(rd_());
		static std::uniform_int_distribution<T> uniformDist_(min, max);
		return uniformDist_(engine_);
	}

}