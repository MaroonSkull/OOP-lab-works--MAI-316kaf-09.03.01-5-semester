#pragma once



namespace Global {
	enum class Direction {
		leftToRight,
		upToDown,
		rightToLeft,
		downToUp
	};

	enum class LineType {
		line,
		zigzag,
		rhombus
	};

	enum class EpilepsyType {
		symbol,
		line
	};

	// настройки
	static constexpr auto myDirection{ Direction::leftToRight };
	static constexpr auto myLineType{ LineType::rhombus };
	static constexpr auto myEpilepsyType{ EpilepsyType::line };
	static constexpr bool enableClearScreen{ false };
}