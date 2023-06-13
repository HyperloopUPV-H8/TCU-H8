#pragma once
#include "TCU_common/TCU_common.hpp"

namespace emergency_button{

void EB_fault(){
	ErrorHandler("emergency button triggered, entering FAULT");
	common::send_to_fault();
}

SensorInterrupt *button;

PinState *button_state = new PinState(OFF);

void inscribe(){
	//TODO: add button declaration with send_to_fault to respective pin (yet unknown): button = new SensorInterrupt(X,IMD_fault,IMDState1);
}

}
