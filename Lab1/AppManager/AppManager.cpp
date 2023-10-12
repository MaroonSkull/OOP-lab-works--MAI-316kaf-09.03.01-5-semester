#include <AppManager.hpp>

AppManager::AppManager() {
	try {
		getConsoleInfo();

		// ���������� ���������� ��������� ����
		/*freq_ = getIntegralFromConsole("������� ��������� �����", 1, 30);
		speed_ = getIntegralFromConsole("�������� �����", 1, 30);
		length_ = getIntegralFromConsole("����� �����", 1, 30);
		epilepsy_ = getConfirmFromConsole("����� ���������");*/

		freq_ = 1;
		speed_ = 1;
		length_ = 5;
		epilepsy_ = false;

		
	}
	catch (...) {
		std::cerr << "���������� � AppManager()!" << std::endl;
		throw;
	}
}

void AppManager::updateScreen(double dt) {
	for (auto it = LineList_.begin(); it != LineList_.end(); it++) {
		// ��� ����� ������ �� �������������, � ��������� ���������� �� n ��������
		it->move(speed_ * dt / 1000.);
		auto offset{ it->getOffset() };
		auto y{ it->getY() };
		if (y - offset < 0) 
			LineList_.erase(it);
	}
	clearScreen();
	for (auto& node : LineList_)
		node.print();
}

void AppManager::addLine() {
	LineList_.push_back(Line(length_, width_, height_, epilepsy_));
}

void AppManager::clearScreen() {
#ifdef __linux__
#elif _WIN32
	COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	GetConsoleScreenBufferInfo(hConsole_, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(hConsole_, ' ', cells, tl, &written);
	FillConsoleOutputAttribute(hConsole_, s.wAttributes, cells, tl, &written);
	SetConsoleCursorPosition(hConsole_, tl);
#endif
}

uint8_t AppManager::getFrequency() const {
	return freq_;
}

bool AppManager::getConfirmFromConsole(std::string_view msg) {
	std::string inp;

	for (;;)
		try {
		using namespace std::literals; // operator ""s

		std::string inp;

		std::cout << "�������� " << msg << "? (Y/N): " << std::endl;
		std::getline(std::cin, inp); // fetch user input, save into inp

		if (std::cin.fail()) throw std::invalid_argument("���� �� ������� ����������������!"); // ������ �����, ��� ���������� ������ ������� �� ���������
		if ((inp != "Y" && inp != "N")) throw std::invalid_argument("�������� �������� ("s + inp + ") �� ��������!");

		// � ���� ������ inp �������������� �������� ���� "Y", ���� "N",
		// ���������� ������� ������ �� ���� �� ���������
		return (inp == "Y") ? true : false;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	catch (...) {
		std::cout << "����������� ����������� ������!" << std::endl;
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
	width_ = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	height_ = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
	// ����� �������� ��������� ��� ���� ��, ��� ������� ��������� ���������� ���� �������� ����� arrv, arrc
#endif
	if (width_ == 0 || height_ == 0)
		throw std::runtime_error{ "AppManager::getConsoleInfo() : �� ������� ���������� ������ �������!" };
}