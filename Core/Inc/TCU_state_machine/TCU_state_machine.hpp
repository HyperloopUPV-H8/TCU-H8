#pragma once
#include "TCU_leds/TCU_board_leds.hpp"
#include "TCU_leds/TCU_tower_leds.hpp"
#include "TCU_common/TCU_common.hpp"
#include "TCU_pressure_sensor/TCU_pressure_sensor.hpp"
#include "TCU_ethernet/TCU_ethernet_udp.hpp"
#include "TCU_ethernet/TCU_ethernet_tcp.hpp"
#include "TCU_pump/TCU_pump.hpp"
#include "TCU_IMD/TCU_IMD.hpp"
#include "TCU_tube_illumination/TCU_tube_illumination.hpp"
using namespace std::chrono_literals;


namespace state_machine{

//######################-----variables-----#############################
StateMachine PrincipalStateMachine;
StateMachine PumpStateMachine;

bool ended_setup = false;
bool initial_timeout = false;


//####################-----check transition methods-----###########################

bool operational_fault_check(){
	/*
	if(pressure_sensor::check_pressure_limits() && is_ready()){
		ErrorHandler("pressure on tube got over the safe limits while TCU pumping system was running, entering FAULT state");
		return true;
	}
	if(pressure_sensor::check_temperature_limits()){
		ErrorHandler("temperature on tube got over the safe limits, entering FAULT state");
		return true;
	}*/
	if(pressure_sensor::get_communication_fault()){
		ErrorHandler("Lost communication with TCU sensors while TCU was operating, entering FAULT state");
		return true;
	}
	return false;
}

bool initial_operational_check(){
	return ended_setup && ethernet::is_connected();
}

bool initial_fault_check(){
	return pressure_sensor::get_communication_fault() || initial_timeout;
}

bool ready_pumping_check(){
	return pressure_sensor::get_pressure() > MAX_PRESSURE_PUMP;
}

bool pumping_ready_check(){
	return pressure_sensor::get_pressure() <= IDEAL_PRESSURE;
}

bool first_pumping_ready_check(){
	return pressure_sensor::get_pressure() <= IDEAL_PRESSURE;
}

//##########################-----entry methods-----###############################

void entry_operational(){
	board_leds::operational_led->turn_on();
	tower_leds::turn_off_all();
	tower_leds::turn_green();
	illumination::turn_on();
}

void entry_fault(){
	board_leds::turn_off_all();
	board_leds::fault_led->turn_on();
	tower_leds::turn_off_all();
	tower_leds::turn_red();
	pump::turn_off();
	illumination::turn_off();
}

void entry_idle(){
	tower_leds::turn_off_orange();
}

void exit_idle(){
	tower_leds::turn_orange();
}


//########################-----init methods-----#############################


void add_transitions(){
	PrincipalStateMachine.add_transition(common::OPERATIONAL,common::FAULT,operational_fault_check);
	PrincipalStateMachine.add_transition(common::INITIAL,common::OPERATIONAL,initial_operational_check);
	PrincipalStateMachine.add_transition(common::INITIAL,common::FAULT,initial_fault_check);
	PumpStateMachine.add_transition(common::READY,common::PUMPING,ready_pumping_check);
	PumpStateMachine.add_transition(common::PUMPING,common::READY,pumping_ready_check);
	PumpStateMachine.add_transition(common::FIRST_PUMPING,common::READY,first_pumping_ready_check);
}

void add_entry(){
	PrincipalStateMachine.add_enter_action(entry_fault, common::FAULT);
	PrincipalStateMachine.add_enter_action(entry_operational, common::OPERATIONAL);
	PumpStateMachine.add_enter_action(entry_idle, common::IDLE);
	PumpStateMachine.add_exit_action(exit_idle, common::IDLE);
}

void add_cyclic(){
	Time::register_low_precision_alarm(16, ethernet::set_pending_communication);
	PrincipalStateMachine.add_low_precision_cyclic_action(pressure_sensor::set_pending_communication, std::chrono::milliseconds(SENSOR_PERIOD_BETWEEN_READS_MILLISECONDS),common::OPERATIONAL);
	PrincipalStateMachine.add_low_precision_cyclic_action(pressure_sensor::set_packet_ready, std::chrono::milliseconds(SENSOR_PACKET_DELAY_MILLISECONDS),common::OPERATIONAL);
}

void init(){
	PrincipalStateMachine.add_state(common::INITIAL);
	PrincipalStateMachine.add_state(common::OPERATIONAL);
	PrincipalStateMachine.add_state(common::FAULT);
	ProtectionManager::link_state_machine(PrincipalStateMachine, common::FAULT);
	ProtectionManager::set_id(Boards::ID::TCU);
	PumpStateMachine.add_state(common::IDLE);
	PumpStateMachine.add_state(common::PUMPING);
	PumpStateMachine.add_state(common::READY);
	PumpStateMachine.add_state(common::FIRST_PUMPING);
	PrincipalStateMachine.add_state_machine(PumpStateMachine, common::OPERATIONAL);
	add_transitions();
	add_entry();
	add_cyclic();
}

//######################-----control methods-----############################

void board_start(){
	ended_setup = true;
	Time::set_timeout(TCP_CONNECTION_TIMEOUT_MILLISECONDS, [&](){
		if(PrincipalStateMachine.current_state == common::INITIAL){
			initial_timeout = true;
		}
	});
}

void update(){
	PrincipalStateMachine.check_transitions();
}

bool is_operational(){
	return PrincipalStateMachine.current_state == common::OPERATIONAL;
}

bool is_ready(){
	return PumpStateMachine.current_state == common::READY || PumpStateMachine.current_state == common::PUMPING;
}


void force_fault(){
	PrincipalStateMachine.force_change_state(common::FAULT);
}

}
