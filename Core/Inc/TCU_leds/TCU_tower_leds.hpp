#pragma once
#include "TCU_common/TCU_common.hpp"


namespace tower_leds{

DigitalOutput *green_led; //off and safe
DigitalOutput *orange_led; //on and working
DigitalOutput *red_led; //FAULT state
DigitalOutput *blue_led; //disconnected from network

void init(){
	green_led = out::out5;
	orange_led = out::out6;
	red_led = out::out7;
	blue_led = out::out8;
}

void turn_green(){
	turn_off_all();
	green_led->turn_on();
}

void turn_orange(){
	turn_off_all();
	orange_led->turn_on();
}

void turn_red(){
	turn_off_all();
	red_led->turn_on();
}

void turn_blue(){
	turn_off_all();
	blue_led->turn_on();
}

void turn_off_all(){
	green_led->turn_off();
	orange_led->turn_off();
	red_led->turn_off();
	blue_led->turn_off();
}

}
