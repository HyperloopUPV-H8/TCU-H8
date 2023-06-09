#pragma once
#include "TCU_state_machine/TCU_state_machine.hpp"

namespace TCU{

/**
 * @brief 	methods to initialize TCU mean to be invoked before STLIB::start()
 */
void inscribe(){
	pressure_sensor::inscribe();
	leds::inscribe();
	IMD::inscribe();
}

/**
 * @brief	methods to initialize TCU meant to be invoked after STLIB::start()
 */
void init(){
	state_machine::init();
	ethernet::start_datagram_socket();
	ethernet::init();
}

	static void start(){
		inscribe();
		STLIB::start(common::TCU_IP_STR);
		init();
		pressure_sensor::setup_communication();
		state_machine::board_start();
	}

	static void update(){
		STLIB::update();
		state_machine::update();
		if(pressure_sensor::communication_is_pending())pressure_sensor::check_sensor();
		if(ethernet::communication_is_pending())ethernet::send_sensor_variables();
	}

}

namespace common{

void send_to_fault(){
	state_machine::force_fault();
}

principal_state_machine_states get_principal_state(){
	return (principal_state_machine_states)state_machine::PrincipalStateMachine.current_state;
}

pump_state_machine_states get_pump_state(){
	return (pump_state_machine_states)state_machine::PumpStateMachine.current_state;
}

void set_pumping_state(pump_state_machine_states state){
	state_machine::PumpStateMachine.force_change_state(state);
}



}
