#pragma once


#include <chrono>
#include <iostream>
#include <string>

#include <Windows.h>

#include <Line.hpp>



// Конфигурируем типы, которые будут относиться ко времени
using namespace std::chrono; // используем пространство имён библиотеки <chrono>
using Clock = steady_clock; // монотонные часы
using TimeAccuracy = std::milli; // достаточно точности в миллисекунду
using Duration = duration<double, TimeAccuracy>; // Класс, отображающий длительность времени
using TimePoint = time_point<Clock, Duration>; // Класс, отображающий точку во времени
const double timeToSeconds = 0.001; // Сколько секунд в миллисекунде


class AppManager {
private:
	// Размеры экрана
	int width = 0;
	int height = 0;

	int freq = 0;
	int speed = 0;
	int length = 0;
	bool epilepsy = false;

	// Хранит линию и, до первого смещения, время её создания
	std::list<std::pair<Line, std::optional<TimePoint>>> Lines;

	// Валидирует ввод, чтобы введённое число находилось на интервале [min, max]
	int getIntegralFromConsole(std::string_view msg, int min, int max);

	bool getConfirmFromConsole(std::string_view msg);

	void updateConsoleSizes();

	void resetConsoleCursorPos();

	void clearScreen();

public:
	AppManager();
	~AppManager() = default;

	void updateScreen(Duration dt);

	void addLine(TimePoint additionTime);

	int getFrequency() const;
};