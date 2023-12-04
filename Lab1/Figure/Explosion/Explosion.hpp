#pragma once


#include <Figure.hpp>



class Explosion : public Figure {
private:
	int8_t R_{};
public:
  Explosion(int16_t width, int16_t height, int8_t R, bool epilepsy);

  virtual void move(Buffer &Buff, double distance);
};