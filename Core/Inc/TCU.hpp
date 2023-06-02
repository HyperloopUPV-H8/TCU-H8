#pragma once
#include "ST-LIB.hpp"
#include "TCU_pressure_sensor/TCU_pressure_sensor.hpp"
#include "TCU_state_machine/TCU_state_machine.hpp"

namespace TCU{

	static void start(){
		pressure_sensor::inscribe();
		leds::inscribe();
		STLIB::start(TCU_IP);
		state_machine::init();
		pressure_sensor::setup_communication();
		ethernet::start_datagram_socket();
		ethernet::start_server_socket();
		state_machine::board_start();
	}

	static void update(){
		STLIB::update();
		state_machine::update();
		if(pressure_sensor::communication_is_pending() && state_machine::is_operational())pressure_sensor::check_sensor();
		if(ethernet::communication_is_pending())ethernet::send_sensor_variables();
	}
}
