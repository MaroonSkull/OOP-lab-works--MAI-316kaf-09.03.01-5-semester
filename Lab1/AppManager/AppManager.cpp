#include <AppManager.hpp>

AppManager::AppManager() {
	try {
		updateConsoleSizes(); // инициируем height_, width_
		
		freq_ = getIntegralFromConsole("частоту появления линий", 1, 30);
		speed_ = getIntegralFromConsole("скорость линий", 1, 30);
		length_ = getIntegralFromConsole("длину линий", 1, 30);
		epilepsy_ = getConfirmFromConsole("режим эпилепсии");

		clearScreen();
	}
	catch (...) {
		clearScreen();
		Global::setConsoleCursorPos(0, 0);
		std::cerr << "Исключение в AppManager()!" << std::endl;
		throw;
	}
}

// dt - прошедшее время в секундах
void AppManager::updateScreen(Buffer &Buff, double dt) {
	// Проверяем, не изменились ли размеры консоли?
	updateConsoleSizes();
	Buff.resize(width_, height_);

	// Для каждой линии
	for (auto it = LineList_.begin(); it != LineList_.end();) {
		// сдвигаем линию туда, где она должна была оказаться с такой скоростью через такое время
		it->move(Buff, speed_ * dt);

		// получаем координаты начала линии
		auto x{ it->getX() };
		auto y{ it->getY() };

		// если координаты начала линии скрылись за пределами отображаемой области
		if (x != std::clamp(x, static_cast<int16_t>(0), width_) ||
			y != std::clamp(y, static_cast<int16_t>(0), height_)) {
			// удаляем линию и обновляем итератор
			it = LineList_.erase(it);
		}
		else
			it++;
	}
	
	// Выводим все линии в порядке их появления (от старых к новым)
	for (auto& node : LineList_)
		node.print(Buff, width_, height_); // Передаём текущие размеры экрана
}

void AppManager::addLine() {
	LineList_.push_back(Line(width_, height_, length_, epilepsy_));
}

void AppManager::clearScreen() {
	CONSOLE_SCREEN_BUFFER_INFO s;
	GetConsoleScreenBufferInfo(Global::hConsole, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(Global::hConsole, ' ', cells, Global::tl, &written);
	FillConsoleOutputAttribute(Global::hConsole, s.wAttributes, cells, Global::tl, &written);
	Global::setConsoleCursorPos(0, 0);
}

int8_t AppManager::getFrequency() const {
	return freq_;
}

bool AppManager::getConfirmFromConsole(std::string_view msg) {
	std::string inp;

	for (;;)
		try {
			using namespace std::literals; // operator ""s

			std::string inp;

			std::cout << "Включить " << msg << "? (Y/N): " << std::endl;
			std::getline(std::cin, inp); // fetch user input, save into inp

			if (std::cin.fail()) throw std::invalid_argument("Ввод не удалось интерпретировать!"); // Скорее всего, это исключение вообще никогда не возникнет
			if ((inp != "Y" && inp != "N")) throw std::invalid_argument("Введённое значение ("s + inp + ") не подходит!");

			// В этот момент inp гарантированно содержит либо "Y", либо "N",
			// достаточно условия только на одно из состояний
			return (inp == "Y") ? true : false;
		}
		catch (const std::exception& e) {
			clearScreen();
			Global::setConsoleCursorPos(0, 0);
			std::cerr << e.what() << std::endl;
		}
		catch (...) {
			clearScreen();
			Global::setConsoleCursorPos(0, 0);
			std::cerr << "Неизвестная критическая ошибка!" << std::endl;
			throw;
		}
}

bool AppManager::updateConsoleSizes() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(Global::hConsole, &csbi);

	auto width{ csbi.srWindow.Right - csbi.srWindow.Left };
	auto height{ csbi.srWindow.Bottom - csbi.srWindow.Top };

	if (width == 0 || height == 0)
		throw std::runtime_error{ "AppManager::getConsoleInfo() : Не удалось определить размер консоли!" };

	if (width != width_ || height_ != height) {
		width_ = width;
		height_ = height;
		return true;
	}

	return false;
}