#pragma once
#include "TCU_common/TCU_supply_in_out.hpp"


namespace illumination{

DigitalOutput *tube_leds;

void init(){
	tube_leds = out::outi;
}

void turn_on(){
	tube_leds->turn_on();
}

void turn_off(){
	tube_leds->turn_off();
}

}
