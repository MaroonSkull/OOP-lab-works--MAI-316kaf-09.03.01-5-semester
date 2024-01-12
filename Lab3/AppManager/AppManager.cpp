#include <AppManager.hpp>
#include <algorithm>

AppManager::AppManager() {
	try {
		updateConsoleSizes(); // инициируем height_, width_

		freq_ = getIntegralFromConsole("frequency", 1, 30);
		speed_ = getIntegralFromConsole("speed", 1, 30);
		length_ = getIntegralFromConsole("length", 1, 30);
		epilepsy_ = getConfirmFromConsole("epilepsy");
		probability_ = getIntegralFromConsole("probability", 1, 1000);
		minR_ = getIntegralFromConsole("minimum radius", 1, 10);
		maxR_ = getIntegralFromConsole("maximum radius", static_cast<int>(minR_), 10);

		clearScreen();
	}
	catch (...) {
		clearScreen();
		Global::setConsoleCursorPos(0, 0);
		std::cerr << "Exception in AppManager()!" << std::endl;
		throw;
	}
}

// dt - прошедшее время
void AppManager::updateScreen(Buffer &Buff, Global::Duration dt) {
	// Проверяем, не изменились ли размеры консоли?
	updateConsoleSizes();
	Buff.resize(width_, height_);

	// Для каждой линии
	for (auto it = LineList_.begin(); it != LineList_.end();) {
		auto &[currentLine, startTimeOpt] = *it;
		// сдвигаем линию туда, где она должна была оказаться с такой скоростью через такое время
		auto ellapsedTime = dt;
		if (startTimeOpt) {
			ellapsedTime += Global::Clock::now() - startTimeOpt.value();
			startTimeOpt = std::nullopt;
		}
		
		bool wasThereAShift {currentLine.move(Buff, speed_ * ellapsedTime.count() * Global::timeToSeconds)};

		// Решаем, должна ли взорваться линия
		if (wasThereAShift && Global::getRandomUniformDistribution(0, 1000) < probability_) {
			auto explosionStartPosition = currentLine.tailCut(); 
			if (explosionStartPosition) {
				auto maxR{ (minR_ == maxR_) ? minR_ : Global::getRandomUniformDistribution(minR_, maxR_) };
				ExplosionList_.push_back(Explosion(width_, height_, maxR, explosionStartPosition.value()));
			}
			else {
				// удаляем линию и обновляем итератор
				it = LineList_.erase(it);
				continue;
			}
		}

		// получаем координаты начала линии
		auto x{ currentLine.getX() };
		auto y{ currentLine.getY() };

		// если координаты начала линии скрылись за пределами отображаемой области
		if (x != std::clamp(x, static_cast<int16_t>(0), width_) ||
			y != std::clamp(y, static_cast<int16_t>(0), height_)) {
			// удаляем линию и обновляем итератор
			it = LineList_.erase(it);
		}
		else
			it++;
	}

	// Двигаем все существующие взрывы с фиксированной скоростью 2 символа в секунду
	for (auto Explosion = ExplosionList_.begin(); Explosion != ExplosionList_.end();) {
		if ((*Explosion).move(Buff, 2 * dt.count() * Global::timeToSeconds))
			Explosion = ExplosionList_.erase(Explosion);
		else
			Explosion++;
	}

	// Передаём текущие размеры экрана
	auto printLine = [this, &Buff](auto &Line) { Line.first.print(Buff, width_, height_); };
	auto printExplosion = [this, &Buff](auto &Explosion) { Explosion.print(Buff, width_, height_); };
	// Выводим все линии и взрывы в порядке их появления (от старых к новым)
	std::for_each(LineList_.begin(), LineList_.end(), printLine);
	std::for_each(ExplosionList_.begin(), ExplosionList_.end(), printExplosion);
}

void AppManager::addLine(Global::TimePoint additionTime) {
	LineList_.push_back(std::make_pair(Line(width_, height_, length_, epilepsy_), additionTime));
}

void AppManager::clearScreen() {
	CONSOLE_SCREEN_BUFFER_INFO s;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', cells, { 0, 0 }, &written);
	FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), s.wAttributes, cells, { 0, 0 }, &written);
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

		std::cout << "Enable " << msg << "? (Y/N): " << std::endl;

		std::getline(std::cin, inp); // fetch user input, save into inp

		if (std::cin.fail()) throw std::invalid_argument("invalid argument!");
		// Приводим строку к нижнему регистру
		std::transform(inp.begin(), inp.end(), inp.begin(),
			[](std::string::value_type c) { return std::tolower(c); });
		// Если введена любая из подходящих команд
		if ((inp != "y" && inp != "n")) throw std::invalid_argument("Entered value ("s + inp + ") is invalid!");

		clearScreen();

		// В этот момент inp гарантированно содержит либо "y", либо "n",
		// достаточно условия только на одно из состояний
		return (inp == "y") ? true : false;
	}
	catch (const std::exception &e) {
		clearScreen();
		Global::setConsoleCursorPos(0, 0);
		std::cerr << e.what() << std::endl;
	}
	catch (...) {
		clearScreen();
		Global::setConsoleCursorPos(0, 0);
		std::cerr << "Unknown critical exeption!" << std::endl;
		throw;
	}
}

bool AppManager::updateConsoleSizes() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	auto width{ csbi.srWindow.Right - csbi.srWindow.Left };
	auto height{ csbi.srWindow.Bottom - csbi.srWindow.Top };

	if (width == 0 || height == 0)
		throw std::runtime_error{ "AppManager::getConsoleInfo() : Sizes of console is undefined!" };

	if (width != width_ || height_ != height) {
		width_ = width;
		height_ = height;
		return true;
	}

	return false;
}