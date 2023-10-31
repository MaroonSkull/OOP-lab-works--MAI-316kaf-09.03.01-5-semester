﻿#include <algorithm> // std::sort
#include <array> // std::array
#include <execution> // автораспараллеливание и автовекторизация циклов
#include <future>
#include <iostream>
#include <thread>

#include <AppManager.hpp>
#include <Buffer.hpp>



int main() {
	using namespace Global;

	// Используем двойную буфферизацию
	std::array<Buffer, 2> Buffers{};
	std::vector<TimePoint> timePoints;

	try {
		// Создаём класс, управляющий консолью и отрисовкой
		AppManager Application;

		// Количество запросов на создание новой линии в секунду
		auto freq = Application.getFrequency();

		// как часто будем создавать новую линию
		// T = 1/f, вспоминаем физику. + конвертируем секунды в нужную нам единицу изменения времени
		Duration period{ 1. / (static_cast<double>(freq) * timeToSeconds) };

		TimePoint frameStartTime = Clock::now(); // как много времени прошло с начала прошлого кадра
		TimePoint secondStartTime = Clock::now(); // как много времени прошло с прошлой секунды

		// Бесконечный цикл отрисовки матрицы со сменой буферов
		auto BuffIt{ Buffers.begin() };
		auto &currentBuffer{ *BuffIt };
		auto rotateBuffers = [&]() {
			// Берём новый буффер, в который будем рисовать
			BuffIt++;
			if (BuffIt == Buffers.end()) BuffIt = Buffers.begin();
			currentBuffer = *BuffIt;

			return std::async(std::launch::async, [&]() { currentBuffer.clear(); });
			};
		auto isPrinted = std::async(std::launch::async, [&]() {currentBuffer.print(); });
		auto isCleared = rotateBuffers();

		while (true) {

			// вычисляем всё время, затраченное на прошлый кадр
			Duration timeFromLastScreenUpdate{ Clock::now() - frameStartTime };

			// Засекаем время начала нового кадра
			frameStartTime = Clock::now();

			isCleared.wait();

			// Отправляем запрос на изменение состояния и отрисовку в выбранный буффер
			Application.updateScreen(currentBuffer, timeFromLastScreenUpdate);

			// Асинхронно отображаем наш буффер
			isPrinted = std::async(std::launch::async, [&]() { currentBuffer.print(); });

			// Обрабатываем все прошедшие секунды, которые мы могли пропустить
			while (Clock::now() - secondStartTime > Duration{ 1.0 / timeToSeconds }) {
				secondStartTime += Duration{ 1.0 / timeToSeconds }; // Добавляем одну секунду
				auto prevSize{ timePoints.size() };
				timePoints.resize(prevSize + freq); // Расширяем хранилище на столько точек, сколько надо добавить на одну секунду
				auto pointsBegin{ timePoints.begin() + prevSize };
				// Добавляем рандомный момент во времени, когда мы должны будем создать линию
				std::for_each(std::execution::par_unseq, pointsBegin, timePoints.end(),
					[&secondStartTime](TimePoint &Point) {
						Point = secondStartTime + Duration(getRandomUniformDistribution(0., 1. / timeToSeconds));
					});
				std::sort(std::execution::par_unseq, pointsBegin, timePoints.end()); // сортируем все точки времени
			}

			// Пока список точек не пуст, добавляем столько линий, сколько должны были добавить за прошедшее время
			while (timePoints.begin() != timePoints.end()) {
				auto &point = timePoints.front();
				// Если время добавления ещё не наступило
				if (point > Clock::now())
					break; // То выходим из цикла, время наступит потом когда-нибудь

				Application.addLine(std::move(point));
				timePoints.erase(timePoints.begin()); // удаляем первый элемент
				timePoints.shrink_to_fit(); // можно просто resize()
			}

			// Вычисляем время, затраченное на текущий кадр
			Duration frameTime{ Clock::now() - frameStartTime };

			// Отображаем текущий FPS
			if constexpr (showFPS) {
				std::thread fps(
					[&frameTime]() {
						setConsoleColor(15);
						std::cout << "FPS = " << 1. / (frameTime.count() * timeToSeconds) << std::endl;
						setConsoleCursorPos(0, 0);
					}
				);
				fps.detach();
			}

			// Если успели быстрее, ждём начала следующего кадра, чтобы не греть кремний
			constexpr Duration minTimePerFrame{ 1. / (Global::maxFPS * timeToSeconds) };
			if (frameTime < minTimePerFrame)
				std::this_thread::sleep_for(minTimePerFrame - frameTime);

			isPrinted.wait();
			isCleared = rotateBuffers();
		}
	}
	catch (const std::exception& e) {
		setConsoleCursorPos(0, 0);
		std::cerr << "std::exception: " << e.what() << std::endl;
		return -1;
	}
	catch (...) {
		setConsoleCursorPos(0, 0);
		std::cerr << "unexpected exception!" << std::endl;
		return -2;
	}

	return 0;
}