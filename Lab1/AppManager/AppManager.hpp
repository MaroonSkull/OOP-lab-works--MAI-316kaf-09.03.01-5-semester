#pragma once

#ifdef __linux__ 
	#include <sys/ioctl.h>
	#include <stdio.h>
	#include <unistd.h>
#elif _WIN32
	#include <windows.h>
#endif

#include <concepts>
#include <functional>
#include <string>
#include <iostream>
#include <Line.hpp>



template<typename T>
concept Integral = std::is_integral_v<T>;

class AppManager {
private:
	// Используем минимально возможные типы
	int16_t width_{};
	int16_t height_{};

	int8_t freq_{};
	int8_t speed_{};
	int8_t length_{};
	bool epilepsy_{};

	std::list<Line<Global::myDirection>> LineList_;

	// Валидирует ввод, чтобы введённое число находилось на интервале [min, max]
	template <Integral _Ty>
	_Ty getIntegralFromConsole(std::string_view msg, _Ty min, _Ty max) {
		auto getIntegralFromString = [](std::string inp) {
			// Выбираем функцию, которой будем получать целое из строки
			std::function<_Ty(const std::string&, size_t*, int)> convert;
			if constexpr (std::is_same_v<_Ty, long long>) {
				convert = static_cast<_Ty(*)(const std::string&, size_t*, int)>(std::stoll);
			}
			else if constexpr (std::is_same_v<_Ty, long>) {
				convert = static_cast<_Ty(*)(const std::string&, size_t*, int)>(std::stol);
			}
			else {
				convert = static_cast<int(*)(const std::string&, size_t*, int)>(std::stoi);
			}
			return convert(inp, nullptr, 10);
		};

		for (;;)
			try {
			using namespace std::literals; // operator ""s

			std::string inp;
			_Ty integer;

			std::cout << "Пожалуйста, введите " << msg << " в интервале [" << min << ", " << max << "]: ";
			std::getline(std::cin, inp); // fetch user input, save into inp
			integer = getIntegralFromString(inp);

			if (std::cin.fail()) throw std::invalid_argument("Ввод не удалось интерпретировать как число!"); // Скорее всего, это исключение вообще никогда не возникнет
			if (integer < min) throw std::out_of_range("Введённое значение ("s + std::to_string(integer) + ") меньше минимально возможного (" + std::to_string(min) + ")!");
			if (integer > max) throw std::out_of_range("Введённое значение ("s + std::to_string(integer) + ") больше максимально возможного (" + std::to_string(max) + ")!");

			return integer;
		}
		catch (const std::exception& e) {
			Global::resetConsoleCursorPos<int>();
			std::cerr << e.what() << std::endl;
		}
		catch (...) {
			Global::resetConsoleCursorPos<int>();
			std::cerr << "Неизвестная критическая ошибка!" << std::endl;
			throw;
		}
	}

	bool getConfirmFromConsole(std::string_view msg);

	void getConsoleInfo();

	void clearScreen();

public:
	AppManager();
	~AppManager() = default;

	void updateScreen(double dt);

	void addLine();

	int8_t getFrequency() const;
};