#include <Explosion.hpp>


Explosion::Explosion(int16_t width, int16_t height, int8_t R, bool epilepsy)
	: Figure(width, height, epilepsy), R_{ R }
{
	
}

void Explosion::move(Buffer &Buff, double distance) {

}