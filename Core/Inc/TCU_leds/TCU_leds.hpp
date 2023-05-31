#pragma once
#include "ST-LIB.hpp"

namespace leds{

DigitalOutput operational_led(PG8);
DigitalOutput fault_led(PG7);
DigitalOutput can_led(PG6);
DigitalOutput flash_led(PG5);
DigitalOutput sleep_led(PG4);

void turn_off_all(){
	operational_led.turn_off();
	fault_led.turn_off();
}

}
