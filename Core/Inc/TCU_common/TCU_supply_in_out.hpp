#pragma once
#include "TCU_common/TCU_common.hpp"

namespace out{
DigitalOutput *out1;
DigitalOutput *out2;
DigitalOutput *out3;
DigitalOutput *out4;
DigitalOutput *out5;
DigitalOutput *out6;
DigitalOutput *out7;
DigitalOutput *out8;
DigitalOutput *outi;


	void inscribe(){
		out1 = new DigitalOutput(PE11);
		out2 = new DigitalOutput(PE12);
		out3 = new DigitalOutput(PE14);
		out4 = new DigitalOutput(PE15);
		out5 = new DigitalOutput(PB14);
		out6 = new DigitalOutput(PB15);
		out7 = new DigitalOutput(PD12);
		out8 = new DigitalOutput(PD13);
		outi = new DigitalOutput(PB12);
		out1->turn_off();
		out2->turn_off();
		out3->turn_off();
		out4->turn_off();
		out5->turn_off();
		out6->turn_off();
		out7->turn_off();
		out8->turn_off();
		outi->turn_off();
	}
}

namespace in{
SensorInterrupt *in1;
SensorInterrupt *in2;
SensorInterrupt *in3;
SensorInterrupt *in4;
PinState in1_value;
PinState in2_value;
PinState in3_value;
PinState in4_value;

void inscribe(){
	in1 = new SensorInterrupt(PG9,common::send_to_fault,&in1_value);
	in2 = new SensorInterrupt(PG10,common::recover_from_fault,&in2_value);
	in3 = new SensorInterrupt(PG12,common::empty_function,&in3_value);
	in4 = new SensorInterrupt(PG14,common::empty_function,&in4_value);
}

}
