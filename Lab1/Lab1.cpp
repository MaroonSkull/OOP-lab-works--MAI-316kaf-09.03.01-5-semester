#include <iostream>
#include <chrono>
#include <AppManager.hpp>
#include <thread>

int main() {
	// Устанавливаем вывод на русском языке
	setlocale(LC_ALL, "Russian");

	try {
		AppManager Application;

		// Количество запросов на создание новой линии в секунду
		auto freq = Application.getFrequency();

		using namespace std::chrono;

		duration<double, std::milli> period{ 1000. / freq };

		time_point<steady_clock, duration<double, std::milli>> additionTime = steady_clock::now();
		auto currentTime = steady_clock::now();
		for (;;) {
			auto frameStartTime = steady_clock::now();
			auto timeFromLastScreenUpdate = std::chrono::duration_cast<duration<double, std::milli>>(currentTime - frameStartTime);
			Application.updateScreen(timeFromLastScreenUpdate.count());
			currentTime = steady_clock::now();
			duration<double, std::milli> timeSinceLastAddition{ currentTime - additionTime };
			if (timeSinceLastAddition > period) {
				// мы могли пропустить несколько запланированных добавлений, добавляем все пропущенные
				while (timeSinceLastAddition > period) {
					timeSinceLastAddition -= period;
					Application.addLine();
				}
				additionTime = steady_clock::now();
				additionTime = additionTime + timeSinceLastAddition;
			}
			duration<double, std::milli> frameTime{ currentTime - frameStartTime };
			std::cout << "FPS = " << 1000.0 / frameTime.count() << std::endl;

			double maxFps = 1.0;
			if (frameTime.count() < 1000. / maxFps) {
				// ждём начала следующего кадра
				duration<double, std::milli> sleepTime(1000./maxFps - frameTime.count());
				auto delta_ms_duration = std::chrono::duration_cast<milliseconds>(sleepTime);
				std::this_thread::sleep_for(milliseconds(delta_ms_duration.count()));
			}

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