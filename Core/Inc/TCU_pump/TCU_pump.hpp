#pragma once
#include "TCU_common/TCU_common.hpp"


namespace pump{

DigitalOutput *pump;

void init(){
	pump = out::out1;
}

void turn_on(){
	pump->turn_on();
}

void turn_off(){
	pump->turn_off();
}
}
