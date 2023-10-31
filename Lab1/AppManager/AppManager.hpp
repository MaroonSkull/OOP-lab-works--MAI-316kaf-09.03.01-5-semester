#pragma once


#include <iostream>
#include <string>

#include <Windows.h>

#include <Line.hpp>




class AppManager {
private:
	// Используем минимально возможные типы
	int16_t width_{};
	int16_t height_{};

	int8_t freq_{};
	int8_t speed_{};
	int8_t length_{};
	bool epilepsy_{};

	// Хранит линию и, до первого смещения, время её создания
	std::list<std::pair<Line, std::optional<Global::TimePoint>>> LineList_;

	// Валидирует ввод, чтобы введённое число находилось на интервале [min, max]
	int getIntegralFromConsole(std::string_view msg, int min, int max);

	bool getConfirmFromConsole(std::string_view msg);

	bool updateConsoleSizes();

	void clearScreen();

public:
	AppManager();
	~AppManager() = default;

	void updateScreen(Global::Duration dt);

	void addLine(Global::TimePoint additionTime);

	int8_t getFrequency() const;
};