#pragma once
#include "TCU_common/TCU_supply_in_out.hpp"


namespace tower_leds{

DigitalOutput *green_led; //off and safe
DigitalOutput *orange_led; //on and working
DigitalOutput *red_led; //FAULT state

void init(){
	green_led = out::out1;
	orange_led = out::out2;
	red_led = out::out3;
}

void turn_off_all(){
	green_led->turn_off();
	orange_led->turn_off();
	red_led->turn_off();
}

void turn_green(){
	green_led->turn_on();
}

void turn_orange(){
	orange_led->turn_on();
}

void turn_off_orange(){
	orange_led->turn_off();
}

void turn_red(){
	red_led->turn_on();
}
}
