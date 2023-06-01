#pragma once
#include "ST-LIB.hpp"
#include "TCU_pressure_sensor/TCU_pressure_sensor.hpp"
#include "TCU_state_machine/TCU_state_machine.hpp"

namespace TCU{

	static void start(){
		pressure_sensor::inscribe();
		leds::inscribe();
		STLIB::start();
		state_machine::init();
		if(!pressure_sensor::setup_communication()){
			state_machine::force_fault();
		}
		ethernet::start_datagram_socket();
		state_machine::board_start();
	}

	static void update(){
		STLIB::update();
		state_machine::update();
	}
}
