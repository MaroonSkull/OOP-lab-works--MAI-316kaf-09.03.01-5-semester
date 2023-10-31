#include <iostream>
#include <chrono>
#include <thread>

#include <AppManager.hpp>
#include <Global.hpp>



int main() {
	using namespace Global;
	std::vector<TimePoint> timePoints;

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

		TimePoint secondStartTime = Clock::now(); // как много времени прошло с прошлой секунды
		TimePoint frameStartTime = Clock::now(); // как много времени прошло с начала прошлого кадра
		// Бесконечный цикл отрисовки матрицы
		for (;;) {
			// вычисляем всё время, затраченное на прошлый кадр
			Duration timeFromLastScreenUpdate{ Clock::now() - frameStartTime };

			// Засекаем время начала нового кадра
			frameStartTime = Clock::now();

			// Отправляем запрос на изменение состояния и отрисовку
			Application.updateScreen(timeFromLastScreenUpdate);

			// Обрабатываем все прошедшие секунды, которые мы могли пропустить
			while (Clock::now() - secondStartTime > Duration{ 1.0 / timeToSeconds }) {
				secondStartTime += Duration{ 1.0 / timeToSeconds }; // Добавляем одну секунду
				auto prevSize{ timePoints.size() };
				timePoints.resize(prevSize + freq); // Расширяем хранилище на столько точек, сколько надо добавить на одну секунду
				auto pointsBegin{ timePoints.begin() + prevSize };
				// Добавляем рандомный момент во времени, когда мы должны будем создать линию
				for(auto &Point = pointsBegin; Point != timePoints.end(); Point++) {
						*Point = secondStartTime + Duration(getRandomUniformRealDistribution(0., 1. / timeToSeconds));
				}
				std::sort(pointsBegin, timePoints.end()); // сортируем все точки времени
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

			// Вычисляем время, затраченное на текущий кадр
			Duration frameTime{ Clock::now() - frameStartTime };
			
			// Отображаем текущий FPS
			if constexpr (Global::showFPS) {
				Global::setConsoleColor(15);
				std::cout << "FPS = " << 1. / (frameTime.count() * timeToSeconds) << std::endl;
				Global::setConsoleCursorPos(0, 0);
			}

			// Если успели быстрее, ждём начала следующего кадра, чтобы не греть кремний
			constexpr Duration minTimePerFrame{ 1. / (Global::maxFPS * timeToSeconds) };
			if (frameTime < minTimePerFrame)
				std::this_thread::sleep_for(minTimePerFrame - frameTime);
		}
	}
	catch (const std::exception& e) {
		Global::setConsoleCursorPos(0, 0);
		std::cerr << "Стандартное исключение: " << e.what() << std::endl;
		return -1;
	}
	catch (...) {
		Global::setConsoleCursorPos(0, 0);
		std::cerr << "Неожиданное исключение!" << std::endl;
		return -2;
	}

	return 0;
}