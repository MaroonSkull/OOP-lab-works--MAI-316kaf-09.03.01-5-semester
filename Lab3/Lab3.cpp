#include "Global.hpp"
#include <algorithm> // std::sort
#include <chrono>
#include <iostream>
#include <thread> // sleep
#include <iostream>

#include <vector>

#include <AppManager.hpp>
#include <Buffer.hpp>



int main() {
	using namespace Global;

	// Используем двойную буфферизацию
	std::vector<Buffer> Buffers{};
	Buffers.push_back(Buffer());
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
		auto getFrameTime = [&frameStartTime]() -> Duration { // Вычисляем время, затраченное на текущий кадр
			return Clock::now() - frameStartTime;
			};
		TimePoint secondStartTime = Clock::now(); // как много времени прошло с прошлой секунды
		auto getRandomTimePointInSecond = [&secondStartTime](TimePoint &Point) {
			Point = secondStartTime + Duration(getRandomUniformDistribution(0., 1. / timeToSeconds));
			};

		// Бесконечный цикл отрисовки матрицы со сменой буферов
		auto BuffIt{ Buffers.begin() };
		auto &currentBuffer{ *BuffIt };

		while (true) {

			// вычисляем всё время, затраченное на прошлый кадр
			Duration timeFromLastScreenUpdate{ Clock::now() - frameStartTime };

			// Засекаем время начала нового кадра
			frameStartTime = Clock::now();

			// Отправляем запрос на изменение состояния и отрисовку в выбранный буффер
			Application.updateScreen(currentBuffer, timeFromLastScreenUpdate);

			// Асинхронно отображаем наш буффер
			currentBuffer.print();

			// Обрабатываем все прошедшие секунды, которые мы могли пропустить
			while (Clock::now() - secondStartTime > Duration{ 1.0 / timeToSeconds }) {
				secondStartTime += Duration{ 1.0 / timeToSeconds }; // Добавляем одну секунду
				auto prevSize{ timePoints.size() };
				for (int i = 0; i < freq; i++)
					timePoints.push_back(TimePoint()); // Расширяем хранилище на столько точек, сколько надо добавить на одну секунду

				std::ranges::subrange newPoints(timePoints.begin() + prevSize, timePoints.end());
				// Добавляем рандомный момент во времени, когда мы должны будем создать линию
				std::ranges::for_each(newPoints, getRandomTimePointInSecond);
				std::ranges::sort(newPoints);
				//timePoints.shrink_to_fit();
			}

			// Пока список точек не пуст, добавляем столько линий, сколько должны были добавить за прошедшее время
			while (timePoints.begin() != timePoints.end()) {
				auto &point = timePoints.front();
				// Если время добавления ещё не наступило
				if (point > Clock::now())
					break; // То выходим из цикла, время наступит потом когда-нибудь

				Application.addLine(point);
				timePoints.erase(timePoints.begin()); // удаляем первый элемент
				timePoints.shrink_to_fit(); // можно просто resize()
			}

			// Берём новый буффер, в который будем рисовать
			BuffIt++;
			if (BuffIt == Buffers.end()) BuffIt = Buffers.begin();
			currentBuffer = *BuffIt;
			currentBuffer.clear();

			// Отображаем текущий FPS
			if constexpr (showFPS) {
				setConsoleColor(15);
				setConsoleCursorPos(0, 0);
				std::cout << "FPS = " << 1. / (duration_cast<Duration>(Clock::now() - frameStartTime).count() * timeToSeconds) << std::endl;
				setConsoleCursorPos(0, 0);
			}

			// Если успели быстрее, ждём начала следующего кадра, чтобы не греть кремний
			constexpr Duration minTimePerFrame{ 1. / (Global::maxFPS * timeToSeconds) };
			if (Clock::now() - frameStartTime < minTimePerFrame)
				std::this_thread::sleep_for(minTimePerFrame - (Clock::now() - frameStartTime));
		}
	}
	catch (const std::exception &e) {
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