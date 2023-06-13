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

	void inscribe(){
		out1 = new DigitalOutput(PE11);
		out2 = new DigitalOutput(PE12);
		out3 = new DigitalOutput(PE14);
		out4 = new DigitalOutput(PE15);
		out5 = new DigitalOutput(PB14);
		out6 = new DigitalOutput(PB15);
		out7 = new DigitalOutput(PD12);
		out8 = new DigitalOutput(PD13);
		out1->turn_off();
		out2->turn_off();
		out3->turn_off();
		out4->turn_off();
		out5->turn_off();
		out6->turn_off();
		out7->turn_off();
		out8->turn_off();
	}
}
