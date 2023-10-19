#include <AppManager.hpp>

AppManager::AppManager() {
	try {
		getConsoleInfo();
		
		/*freq_ = getIntegralFromConsole("частоту появления линий", 1, 30);
		speed_ = getIntegralFromConsole("скорость линий", 1, 30);
		length_ = getIntegralFromConsole("длину линий", 1, 30);
		epilepsy_ = getConfirmFromConsole("режим эпилепсии");*/

		freq_ = 5;
		speed_ = 20;
		length_ = 18;
		epilepsy_ = false;
	}
	catch (...) {
		Global::resetConsoleCursorPos<int>();
		std::cerr << "Исключение в AppManager()!" << std::endl;
		throw;
	}
}

// dt - прошедшее время в секундах
void AppManager::updateScreen(double dt) {
	// Проверяем, не изменились ли размеры консоли?
	getConsoleInfo();

	// Для каждой линии
	for (auto it = LineList_.begin(); it != LineList_.end();) {
		// сдвигаем линию туда, где она должна была оказаться с такой скоростью через такое время
		it->move(speed_ * dt);

		// получаем координаты начала линии
		auto x{ it->getX() };
		auto y{ it->getY() };

		// если координаты начала линии скрылись за пределами отображаемой области
		if (x != std::clamp(x, static_cast<int16_t>(0), width_) ||
			y != std::clamp(y, static_cast<int16_t>(0), height_)) {
			// удаляем линию
			it = LineList_.erase(it);
		}
		else
			it++;
	}
	// после окончания основной логики, очищаем экран
	clearScreen(); // Это решение годится только для низкого FPS, < ~15, либо для двойной буферизации
	// Выводим все линии в порядке их появления (от старых к новым)
	for (auto& node : LineList_)
		node.print(width_, height_); // Передаём текущие размеры экрана
}

void AppManager::addLine() {
	LineList_.push_back(Line<Global::myDirection, Global::myLineType>(width_, height_, length_, epilepsy_));
}

void AppManager::clearScreen() {
#ifdef __linux__
#elif _WIN32
	CONSOLE_SCREEN_BUFFER_INFO s;
	GetConsoleScreenBufferInfo(Global::hConsole, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(Global::hConsole, ' ', cells, Global::tl, &written);
	FillConsoleOutputAttribute(Global::hConsole, s.wAttributes, cells, Global::tl, &written);
	Global::resetConsoleCursorPos<int>();
#endif
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
			Global::resetConsoleCursorPos<int>();
			std::cerr << e.what() << std::endl;
		}
		catch (...) {
			Global::resetConsoleCursorPos<int>();
			std::cerr << "Неизвестная критическая ошибка!" << std::endl;
			throw;
		}
}

void AppManager::getConsoleInfo() {
#ifdef __linux__
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	width_ = w.ws_row;
	height_ = w.ws_col;
#elif _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	width_ = csbi.srWindow.Right - csbi.srWindow.Left;
	height_ = csbi.srWindow.Bottom - csbi.srWindow.Top;
#else
	// Можно добавить обработку для иных ОС, или сделать возможным мануальный ввод размеров через argc, argv
	width_ = 0;
	height_ = 0;
#endif
	if (width_ == 0 || height_ == 0)
		throw std::runtime_error{ "AppManager::getConsoleInfo() : Не удалось определить размер консоли!" };
}