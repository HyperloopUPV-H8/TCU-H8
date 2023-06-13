#pragma once
#include "TCU_common/TCU_common.hpp"


namespace illumination{

DigitalOutput *tube_leds;

void inscribe(){
	tube_leds = new DigitalOutput(PB12);
}

void turn_on(){
	tube_leds->turn_on();
}

void turn_off(){
	tube_leds->turn_off();
}

}
