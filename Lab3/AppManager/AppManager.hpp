#pragma once


#include <cstdint>
#include <functional>
#include <string>
#include <iostream>

#include <Windows.h>

#include <LinkedList.hpp>

#include <Line.hpp>
#include <Explosion.hpp>
#include <Buffer.hpp>



class AppManager {
private:
	// Используем минимально возможные типы
	int16_t width_{};
	int16_t height_{};

	int8_t freq_{};
	int8_t speed_{};
	int8_t length_{};
	bool epilepsy_{};
	int16_t probability_{};
	int16_t minR_{};
	int16_t maxR_{};

	// Хранит линию и, до первого смещения, время её создания
	LinkedList<std::pair<Line, std::optional<Global::TimePoint>>> LineList_;
	LinkedList<Explosion> ExplosionList_;

	// Валидирует ввод, чтобы введённое число находилось на интервале [min, max]
	template <std::integral _Ty>
	_Ty getIntegralFromConsole(std::string_view msg, _Ty min, _Ty max) {
		auto getIntegralFromString = [](std::string inp) {
			// Выбираем функцию, которой будем получать целое из строки
			std::function<_Ty(const std::string &, size_t *, int)> convert;
			if constexpr (std::is_same_v<_Ty, long long>) {
				convert = static_cast<_Ty(*)(const std::string &, size_t *, int)>(std::stoll);
			}
			else if constexpr (std::is_same_v<_Ty, long>) {
				convert = static_cast<_Ty(*)(const std::string &, size_t *, int)>(std::stol);
			}
			else {
				convert = static_cast<int(*)(const std::string &, size_t *, int)>(std::stoi);
			}
			return convert(inp, nullptr, 10);
			};

		for (;;)
			try {
			using namespace std::literals; // operator ""s

			std::string inp;
			_Ty integer;
			
			std::cout << "Please, enter " << msg << " in [" << min << ", " << max << "]: ";
			std::getline(std::cin, inp); // fetch user input, save into inp

			integer = getIntegralFromString(inp);

			if (std::cin.fail()) throw std::invalid_argument("invalid argument!");
			if (integer < min) throw std::out_of_range("Entered value ("s + std::to_string(integer) + ") less than the minimum possible (" + std::to_string(min) + ")!");
			if (integer > max) throw std::out_of_range("Entered value ("s + std::to_string(integer) + ") more than the maximum possible (" + std::to_string(max) + ")!");

			clearScreen();

			return integer;
		}
		catch (const std::exception &e) {
			clearScreen();
			Global::setConsoleCursorPos(0, 0);
			std::cerr << e.what() << std::endl;
		}
		catch (...) {
			clearScreen();
			Global::setConsoleCursorPos(0, 0);
			std::cerr << "Unknown critical error!" << std::endl;
			throw;
		}
	}

	bool getConfirmFromConsole(std::string_view msg);

	bool updateConsoleSizes();

	void clearScreen();

public:
	AppManager();
	~AppManager() = default;

	void updateScreen(Buffer &Buff, Global::Duration dt);

	void addLine(Global::TimePoint additionTime);

	int8_t getFrequency() const;
};