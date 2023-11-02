#include <iostream>
#include <random>

#include <Windows.h>

#include <AppManager.hpp>



int main() {
	std::vector<TimePoint> timePoints;

	// Устанавливаем вывод на русском языке
	setlocale(LC_ALL, "Russian");

	// Создаём класс, управляющий консолью и отрисовкой
	AppManager Application;

	// Количество запросов на создание новой линии в секунду
	auto freq = Application.getFrequency();

	TimePoint secondStartTime = Clock::now(); // как много времени прошло с прошлой секунды
	TimePoint frameStartTime = Clock::now(); // как много времени прошло с начала прошлого кадра


	// Бесконечный цикл отрисовки
	while (true) {
		// вычисляем всё время, затраченное на прошлый кадр
		Duration timeFromLastScreenUpdate = Clock::now() - frameStartTime;

		// Засекаем время начала нового кадра
		frameStartTime = Clock::now();

		// Отправляем запрос на изменение состояния и отрисовку
		Application.updateScreen(timeFromLastScreenUpdate);

		// Обрабатываем все прошедшие секунды, которые мы могли пропустить
		while (Clock::now() - secondStartTime > Duration(1.0 / timeToSeconds)) {
			secondStartTime += Duration(1.0 / timeToSeconds); // Добавляем одну секунду
			timePoints.reserve(timePoints.size() + freq); // Расширяем хранилище на столько точек, сколько надо добавить на одну секунду
			// Добавляем рандомный момент во времени, когда мы должны будем создать линию
			for (int i = 0; i < freq; i++) {
				static std::random_device rd;
				static std::ranlux24_base engine(rd());
				static std::uniform_real_distribution<double> Distribution(0., 1. / timeToSeconds);
				timePoints.push_back(secondStartTime + Duration(Distribution(engine)));
			}
			std::sort(timePoints.begin(), timePoints.end()); // сортируем все моменты времени
		}

		// Пока список точек не пуст, добавляем столько линий, сколько должны были добавить за прошедшее время
		while (timePoints.begin() != timePoints.end()) {
			auto &point = timePoints.front();
			// Если время добавления ещё не наступило
			if (point > Clock::now())
				break; // То выходим из цикла, время наступит потом когда-нибудь

			Application.addLine(point);
			timePoints.erase(timePoints.begin()); // удаляем первый элемент
		}

		// Вычисляем время, затраченное на текущий кадр
		Duration frameTime = Clock::now() - frameStartTime;

		// Если успели быстрее, ждём начала следующего кадра
		const double maxFPS = 60;
		const Duration minTimePerFrame(1 / (maxFPS * timeToSeconds));
		if (frameTime < minTimePerFrame)
			Sleep((minTimePerFrame - frameTime).count() * timeToSeconds);
	}

	return 0;
}