#pragma once
#include "ST-LIB.hpp"
#include "TCU_leds/TCU_leds.hpp"
#include "TCU_pressure_sensor/TCU_pressure_sensor.hpp"
#include "TCU_ethernet/TCU_ethernet_udp.hpp"
#include "TCU_ethernet/TCU_ethernet_tcp.hpp"
using namespace std::chrono_literals;


namespace state_machine{

//######################-----variables-----#############################
StateMachine PrincipalStateMachine;
StateMachine PumpStateMachine;

bool ended_setup = false;
bool initial_timeout = false;

//####################-----enum of states-----###########################

enum principal_state_machine_states{
	INITIAL,
	OPERATIONAL,
	FAULT,
};

enum pump_state_machine_states{
	IDLE,
	OVER,
	UNDER,
};

//####################-----check transition methods-----###########################

bool operational_fault_check(){
	if(pressure_sensor::check_pressure_limits()){
		ErrorHandler("pressure on tube got over the safe limits, entering FAULT state");
		return true;
	}
	if(pressure_sensor::get_communication_fault()){
		ErrorHandler("Lost communication with TCU sensors, entering FAULT state");
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

bool idle_over_check(){
	return pressure_sensor::get_pressure() > MAX_PRESSURE_PUMP;
}

bool over_idle_check(){
	return pressure_sensor::get_pressure() <= IDEAL_PRESSURE;
}

bool idle_under_check(){
	return pressure_sensor::get_pressure() < MIN_PRESSURE_PUMP;
}

bool under_idle_check(){
	return pressure_sensor::get_pressure() >= IDEAL_PRESSURE;
}

//##########################-----entry methods-----###############################

void entry_operational(){
	leds::operational_led->turn_on();
}

void entry_fault(){
	leds::turn_off_all();
	leds::fault_led->turn_on();
}


//########################-----init methods-----#############################


void add_transitions(){
	PrincipalStateMachine.add_transition(OPERATIONAL,FAULT,operational_fault_check);
	PrincipalStateMachine.add_transition(INITIAL,OPERATIONAL,initial_operational_check);
	PrincipalStateMachine.add_transition(INITIAL,FAULT,initial_fault_check);
	PumpStateMachine.add_transition(IDLE,OVER,idle_over_check);
	PumpStateMachine.add_transition(OVER,IDLE,over_idle_check);
	PumpStateMachine.add_transition(IDLE,UNDER,idle_under_check);
	PumpStateMachine.add_transition(UNDER,IDLE,under_idle_check);
}

void add_entry(){
	PrincipalStateMachine.add_enter_action(entry_fault, FAULT);
	PrincipalStateMachine.add_enter_action(entry_operational, OPERATIONAL);
}

void add_cyclic(){
	PrincipalStateMachine.add_low_precision_cyclic_action(ethernet::set_pending_communication, 16ms,OPERATIONAL);
	PrincipalStateMachine.add_low_precision_cyclic_action(pressure_sensor::set_pending_communication, std::chrono::milliseconds(SENSOR_PERIOD_BETWEEN_READS_MILLISECONDS),OPERATIONAL);
	PrincipalStateMachine.add_low_precision_cyclic_action(pressure_sensor::set_packet_ready, std::chrono::milliseconds(SENSOR_PACKET_DELAY_MILLISECONDS),OPERATIONAL);
}

void init(){
	PrincipalStateMachine.add_state(INITIAL);
	PrincipalStateMachine.add_state(OPERATIONAL);
	PrincipalStateMachine.add_state(FAULT);
	ProtectionManager::link_state_machine(PrincipalStateMachine, FAULT);
	ProtectionManager::set_id(Boards::ID::TCU);
	PumpStateMachine.add_state(IDLE);
	PumpStateMachine.add_state(OVER);
	PumpStateMachine.add_state(UNDER);
	PrincipalStateMachine.add_state_machine(PumpStateMachine, OPERATIONAL);
	add_transitions();
	add_entry();
	add_cyclic();
}

//######################-----control methods-----############################

void board_start(){
	ended_setup = true;
	Time::set_timeout(TCP_CONNECTION_TIMEOUT_MILLISECONDS, [&](){
		if(PrincipalStateMachine.current_state == INITIAL){
			initial_timeout = true;
		}
	});
}

void update(){
	PrincipalStateMachine.check_transitions();
	PumpStateMachine.check_transitions();
}

void force_fault(){
	PrincipalStateMachine.force_change_state(FAULT);
}

bool is_operational(){
	return PrincipalStateMachine.current_state == OPERATIONAL;
}

}
