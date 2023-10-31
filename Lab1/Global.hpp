#pragma once

#include <chrono>
#include <random>

#include <Windows.h>



namespace Global {

	// Конфигурируем типы, которые будут относиться ко времени
	using namespace std::chrono; // используем пространство имён библиотеки <chrono>
	using Clock = steady_clock; // монотонные часы
	using TimeAccuracy = std::milli; // достаточно точности в миллисекунду
	using Duration = duration<double, TimeAccuracy>; // Класс, отображающий длительность времени
	using TimePoint = time_point<Clock, Duration>; // Класс, отображающий точку во времени
	// константа для конвертирования времени. Размерность = [секунды/TimeAccuracy]
	constexpr auto timeToSeconds = static_cast<double>(TimeAccuracy::num) / static_cast<double>(TimeAccuracy::den);

	constexpr auto maxFPS = 60.0;
	static constexpr bool showFPS{ false };

	enum class Direction {
		leftToRight,
		upToDown,
		rightToLeft,
		downToUp
	};

	enum class LineType {
		line,
		zigzag,
		rhombus
	};

	enum class EpilepsyType {
		symbol,
		line
	};

	static constexpr auto myDirection{ Direction::upToDown };
	static constexpr auto myLineType{ LineType::rhombus };
	static constexpr auto myEpilepsyType{ EpilepsyType::line };
	static constexpr bool enableClearScreen{ true };


	const static COORD tl = { 0, 0 };
	const static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	template <typename T>
	void setConsoleColor(T ForgC) {
		WORD wColor = static_cast<WORD>(ForgC & 0x0F);
		SetConsoleTextAttribute(hConsole, wColor);
	}

	template <typename T>
	void setConsoleCursorPos(T x, T y) {
		COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
		SetConsoleCursorPosition(hConsole, pos);
	}

	template <typename T>
	T getRandomUniformIntDistribution(T min, T max) {
		static std::random_device rd_;
		static std::ranlux24_base engine_(rd_());
		std::uniform_int_distribution<T> uniformDist_(min, max);
		return uniformDist_(engine_);
	}

	template <typename T>
	T getRandomUniformRealDistribution(T min, T max) {
		static std::random_device rd_;
		static std::ranlux24_base engine_(rd_());
		std::uniform_real_distribution<T> uniformDist_(min, max);
		return uniformDist_(engine_);
	}
}