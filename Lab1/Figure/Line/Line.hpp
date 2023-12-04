#pragma once


#include <Figure.hpp>



class Line : public Figure {
private:
    bool toggle_{};

	int8_t length_{};
public:
  Line(int16_t width, int16_t height, int8_t length, bool epilepsy);

  virtual void move(Buffer &Buff, double distance);
};