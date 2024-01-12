#pragma once


#include <Figure.hpp>
#include <cstdint>



class Explosion : public Figure {
private:
	double R_{};
  int8_t maxR_{};
public:
  Explosion(int16_t width, int16_t height, int8_t maxR, std::pair<double, double> startPosition);

  virtual int8_t generateColor();

  virtual bool move(Buffer &Buff, double distance);
};