#pragma once
#include "TCU_common/TCU_supply_in_out.hpp"


namespace pump{

DigitalOutput *pump;

void init(){
	pump = out::out4;
}

void turn_on(){
	pump->turn_on();
}

void turn_off(){
	pump->turn_off();
}
}
