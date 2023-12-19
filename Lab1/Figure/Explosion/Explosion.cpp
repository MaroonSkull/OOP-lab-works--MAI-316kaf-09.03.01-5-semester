#include <Explosion.hpp>
#include <algorithm>


Explosion::Explosion(int16_t width, int16_t height, int8_t maxR, std::pair<double, double> startPosition)
	: Figure(width, height, startPosition.first, startPosition.second), maxR_{ maxR }
{}

// Возвращает true, когда радиус достиг максимального значения
bool Explosion::move(Buffer &Buff, double distance) {
	R_ += distance;
	if (R_ >= maxR_)
		return true;

	// Очищаем все ранее созданные части окружности
	Symbols_.clear();

	// Локализуем прямоугольник внутри экрана, в котором будем искать точки окружности
	int16_t minX = std::clamp(std::round(x_ - R_), 0.0, static_cast<double>(width_));
	int16_t maxX = std::clamp(std::round(x_ + R_), 0.0, static_cast<double>(width_));
	int16_t minY = std::clamp(std::round(y_ - R_), 0.0, static_cast<double>(height_));
	int16_t maxY = std::clamp(std::round(y_ + R_), 0.0, static_cast<double>(height_));

	// Проверяем, попадает ли точка окружности хотя бы примерно в клетку
	for (auto x = minX; x < maxX; x++)
		for (auto y = minY; y < maxY; y++) {
			auto X = (x-x_);
			auto Y = (y-y_);

			auto xxyy = X*X+Y*Y;
			auto RR = R_*R_;
			auto rr = (R_-1)*(R_-1);

			if ((xxyy < RR) && (xxyy > rr)) {
				xOffsetCounter_ = std::round(x-x_);
				yOffsetCounter_ = std::round(y-y_);
				addSymbol();
			}
		}
	return false;
}