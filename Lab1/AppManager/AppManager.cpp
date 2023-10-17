#include <AppManager.hpp>

AppManager::AppManager() {
	try {
		getConsoleInfo();

		
		/*freq_ = getIntegralFromConsole("������� ��������� �����", 1, 30);
		speed_ = getIntegralFromConsole("�������� �����", 1, 30);
		length_ = getIntegralFromConsole("����� �����", 1, 30);
		epilepsy_ = getConfirmFromConsole("����� ���������");*/

		freq_ = 1;
		speed_ = 1;
		length_ = 5;
		epilepsy_ = true;

		
	}
	catch (...) {
		std::cerr << "���������� � AppManager()!" << std::endl;
		throw;
	}
}

// dt - ��������� ����� � ��������
void AppManager::updateScreen(double dt) {
	// ���������, �� ���������� �� ������� �������?
	getConsoleInfo();

	// ��� ������ �����
	for (auto it = LineList_.begin(); it != LineList_.end(); it++) {
		// �������� ����� ����, ��� ��� ������ ���� ��������� � ����� ��������� ����� ����� �����
		it->move(speed_ * dt);

		// �������� ���������� ������ �����
		auto x{ it->getX() };
		auto y{ it->getY() };

		// ���� ���������� ������ ����� �������� �� ��������� ������������ �������
		if (x != std::clamp(x, static_cast<uint16_t>(0), width_) &&
			y != std::clamp(y, static_cast<uint16_t>(0), height_)) {

			// ������� �����
			LineList_.erase(it);
		}
	}
	// ����� ��������� �������� ������, ������� �����
	clearScreen();
	// ������� ��� ����� � ������� �� ��������� (�� ������ � �����)
	for (auto& node : LineList_)
		node.print(width_, height_); // ������� ������� ������� ������
}

void AppManager::addLine() {
	LineList_.push_back(Line(width_, height_, length_, epilepsy_));
}

void AppManager::clearScreen() {
#ifdef __linux__
#elif _WIN32
	
	CONSOLE_SCREEN_BUFFER_INFO s;
	GetConsoleScreenBufferInfo(Global::hConsole, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(Global::hConsole, ' ', cells, Global::tl, &written);
	FillConsoleOutputAttribute(Global::hConsole, s.wAttributes, cells, Global::tl, &written);
	SetConsoleCursorPosition(Global::hConsole, Global::tl);
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
	height_ = csbi.srWindow.Bottom - csbi.srWindow.Top;
#else
	// ����� �������� ��������� ��� ���� ��, ��� ������� ��������� ���������� ���� �������� ����� argc, argv
	width_ = 0;
	height_ = 0;
#endif
	if (width_ == 0 || height_ == 0)
		throw std::runtime_error{ "AppManager::getConsoleInfo() : �� ������� ���������� ������ �������!" };
}