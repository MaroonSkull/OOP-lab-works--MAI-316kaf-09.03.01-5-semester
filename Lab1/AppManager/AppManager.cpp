#include <AppManager.hpp>

#include <algorithm>



const static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

AppManager::AppManager() {
	// Получаем текущие размеры окна
	updateConsoleSizes();

	// Получаем данные от пользователя
	freq = getIntegralFromConsole("частоту появления линий", 1, 30);
	speed = getIntegralFromConsole("скорость линий", 1, 30);
	length = getIntegralFromConsole("длину линий", 1, 30);
	epilepsy = getConfirmFromConsole("режим эпилепсии");

	// Очищаем экран
	clearScreen();
}

// dt - прошедшее время в секундах
void AppManager::updateScreen(Duration dt) {
	// Проверяем, не изменились ли размеры консоли?
	updateConsoleSizes();

	auto iterator = Lines.begin();
	// Для каждой линии
	while (iterator != Lines.end()) {
		auto &[currentLine, startTimeOpt] = *iterator;
		// сдвигаем линию туда, где она должна была оказаться с такой скоростью через такое время
		if (startTimeOpt) {
			dt += Clock::now() - startTimeOpt.value();
			startTimeOpt = std::nullopt;
		}

		currentLine.move(speed * dt.count() * timeToSeconds);

		// если координаты начала линии скрылись за пределами отображаемой области
		if (!currentLine.isCoordInsideFrame(currentLine.getX(), currentLine.getY())) {
			// удаляем линию и обновляем итератор
			iterator = Lines.erase(iterator);
		}
		else
			iterator++;
	}

	// после окончания основной логики, очищаем экран
	if constexpr (Global::enableClearScreen)
		clearScreen(); // Это решение годится только для низкого FPS, < ~15

	// Выводим все линии в порядке их появления (от старых к новым)
	for (auto &Line : Lines)
		Line.first.print(width, height); // Передаём текущие размеры экрана
}

void AppManager::addLine(TimePoint additionTime) {
	Lines.push_back(std::make_pair(Line(width, height, length, epilepsy), additionTime));
}

void AppManager::clearScreen() {
	CONSOLE_SCREEN_BUFFER_INFO s;
	COORD tl(0, 0);
	GetConsoleScreenBufferInfo(hConsole, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(hConsole, ' ', cells, tl, &written);
	FillConsoleOutputAttribute(hConsole, s.wAttributes, cells, tl, &written);
}

int AppManager::getFrequency() const {
	return freq;
}

int AppManager::getIntegralFromConsole(std::string_view msg, int min, int max) {
	while (true)
		try {
		using namespace std::literals; // operator ""s

		std::string inp;
		int integer;

		std::cout << "Пожалуйста, введите " << msg << " в интервале [" << min << ", " << max << "]: ";
		std::getline(std::cin, inp); // fetch user input, save into inp
		integer = std::stoi(inp);

		if (std::cin.fail()) throw std::invalid_argument("Ввод не удалось интерпретировать как число!"); // Скорее всего, это исключение вообще никогда не возникнет
		if (integer < min) throw std::out_of_range("Введённое значение ("s + std::to_string(integer) + ") меньше минимально возможного (" + std::to_string(min) + ")!");
		if (integer > max) throw std::out_of_range("Введённое значение ("s + std::to_string(integer) + ") больше максимально возможного (" + std::to_string(max) + ")!");

		return integer;
	}
	catch (const std::exception &e) {
		clearScreen();
		resetConsoleCursorPos();
		std::cerr << e.what() << std::endl;
	}
}

bool AppManager::getConfirmFromConsole(std::string_view msg) {
	std::string inp;

	while (true)
		try {
		using namespace std::literals; // operator ""s

		std::string inp;

		std::cout << "Включить " << msg << "? (Y/N): " << std::endl;
		std::getline(std::cin, inp); // fetch user input, save into inp

		// Скорее всего, это исключение вообще никогда не возникнет
		if (std::cin.fail()) throw std::invalid_argument("Ввод не удалось интерпретировать!");
		// Приводим строку к нижнему регистру
		std::transform(inp.begin(), inp.end(), inp.begin(),
			[](std::string::value_type c) { return std::tolower(c); });
		// Если не введена подходящая команда
		if ((inp != "y" && inp != "n")) throw std::invalid_argument("Введённое значение ("s + inp + ") не подходит!");

		// В этот момент inp гарантированно содержит либо "y", либо "n",
		// достаточно условия только на одно из состояний
		return (inp == "y") ? true : false;
	}
	catch (const std::exception &e) {
		clearScreen();
		resetConsoleCursorPos();
		std::cerr << e.what() << std::endl;
	}
}

void AppManager::updateConsoleSizes() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	width = csbi.srWindow.Right - csbi.srWindow.Left;
	height = csbi.srWindow.Bottom - csbi.srWindow.Top;
}

void AppManager::resetConsoleCursorPos() {
	COORD tl(0, 0);
	SetConsoleCursorPosition(hConsole, tl);
}
