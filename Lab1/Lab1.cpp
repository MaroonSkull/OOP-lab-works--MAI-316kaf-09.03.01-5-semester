﻿#include <iostream>
#include <chrono>
#include <AppManager.hpp>
#include <thread>

int main() {
	// Конфигурируем типы, которые будут относиться ко времени
	using namespace std::chrono; // используем пространство имён библиотеки <chrono>
	using Clock = steady_clock; // монотонные часы
	using TimeAccuracy = std::milli; // достаточно точности в миллисекунду
	using Duration = duration<double, TimeAccuracy>; // Класс, отображающий длительность времени
	using TimePoint = time_point<Clock, Duration>; // Класс, отображающий точку во времени

	// константа для конвертирования времени. Размерность = [секунды/TimeAccuracy]
	constexpr auto timeToSeconds = static_cast<double>(TimeAccuracy::num) / static_cast<double>(TimeAccuracy::den);

	// Устанавливаем вывод на русском языке
	setlocale(LC_ALL, "Russian");

	try {
		// Создаём класс, управляющий консолью и отрисовкой
		AppManager Application;

		// Количество запросов на создание новой линии в секунду
		auto freq = Application.getFrequency();
		// как часто будем создавать новую линию
		// T = 1/f, вспоминаем физику. + конвертируем секунды в нужную нам единицу изменения времени
		Duration period{ 1. / (static_cast<double>(freq) * timeToSeconds) };

		TimePoint additionTime = Clock::now(); // время, когда мы в последний раз добавляли новую линию
		TimePoint frameStartTime = Clock::now(); // как много времени прошло с начала прошлого кадра
		// Бесконечный цикл отрисовки матрицы
		for (;;) {
			// вычисляем всё время, затраченное на прошлый кадр
			Duration timeFromLastScreenUpdate{ Clock::now() - frameStartTime };

			// Засекаем время начала нового кадра
			TimePoint frameStartTime = Clock::now();

			// Отправляем запрос на изменение состояния и отрисовку
			Application.updateScreen(timeFromLastScreenUpdate.count());

			// Если время, прошедшее с прошлого добавления линии больше, чем период
			Duration timeSinceLastAddition{ Clock::now() - additionTime };
			if (timeSinceLastAddition > period) {
				// то добавляем столько линий, сколько должны были добавить за прошедшее время
				while (timeSinceLastAddition > period) {
					timeSinceLastAddition -= period;
					Application.addLine();
				}

				// обновляем время добавления линии
				additionTime = Clock::now(); // Но делаем это так, будто мы добавили линии не когда получилось у процессора,
				additionTime = additionTime - timeSinceLastAddition; // а когда это было необходимо с точки зрения физики
			}

			// Вычисляем время, затраченное на текущий кадр
			Duration frameTime{ Clock::now() - frameStartTime };

			COORD tl = { 0,0 };
			CONSOLE_SCREEN_BUFFER_INFO s;
			HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(console, &s);
			SetConsoleCursorPosition(console, tl);
			std::cout << "FPS = " << 1. / (frameTime.count() * timeToSeconds) << std::endl;

			constexpr auto maxFPS = 1.0;
			constexpr Duration minTimePerFrame{ 1. / (maxFPS * timeToSeconds) };
			// Если успели быстрее, ждём начала следующего кадра, чтобы не греть кремний
			if (frameTime < minTimePerFrame)
				std::this_thread::sleep_for(minTimePerFrame - frameTime);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Стандартное исключение: " << e.what() << std::endl;
		return -1;
	}
	catch (...) {
		std::cerr << "Неожиданное исключение!" << std::endl;
		return -2;
	}

	return 0;
}