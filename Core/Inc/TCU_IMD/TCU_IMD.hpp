#pragma once
#include "TCU_common/TCU_common.hpp"

namespace IMD{

void IMD_fault(){
	ErrorHandler("Insulation Monitoring Device triggered, entering FAULT");
	common::send_to_fault();
}

SensorInterrupt *IMD1;
SensorInterrupt *IMD2;
SensorInterrupt *IMD3;
SensorInterrupt *IMD4;

PinState *IMDState1 = new PinState;
PinState *IMDState2 = new PinState;
PinState *IMDState3 = new PinState;
PinState *IMDState4 = new PinState;

void inscribe(){
	IMD1 = new SensorInterrupt(PG9,IMD_fault,IMDState1);
	IMD2 = new SensorInterrupt(PG10,IMD_fault,IMDState2);
	IMD3 = new SensorInterrupt(PG12,IMD_fault,IMDState3);
	IMD4 = new SensorInterrupt(PG14,IMD_fault,IMDState4);
}

}
