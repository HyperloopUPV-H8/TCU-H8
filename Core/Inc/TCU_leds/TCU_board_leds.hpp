#pragma once
#include "TCU_common/TCU_common.hpp"

namespace board_leds{

DigitalOutput *operational_led;
DigitalOutput *fault_led;
DigitalOutput *can_led;
DigitalOutput *flash_led;
DigitalOutput *sleep_led;

void inscribe(){
	operational_led = new DigitalOutput(PG8);
	fault_led = new DigitalOutput(PG7);
	can_led = new DigitalOutput(PG6);
	flash_led = new DigitalOutput(PG5);
	sleep_led = new DigitalOutput(PG4);
}

void turn_off_all(){
	operational_led->turn_off();
	fault_led->turn_off();
	can_led->turn_off();
	flash_led->turn_off();
	sleep_led->turn_off();
}

}
