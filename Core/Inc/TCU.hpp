#pragma once
#include "ST-LIB.hpp"
#include "TCU_pressure_sensor/TCU_pressure_sensor.hpp"
#include "TCU_state_machine/TCU_state_machine.hpp"

namespace TCU{

	static void start(){
		pressure_sensor::inscribe();
		STLIB::start();
		state_machine::init();
		state_machine::board_start();
	}

	static void update(){
		STLIB::update();
		state_machine::update();
	}
}
