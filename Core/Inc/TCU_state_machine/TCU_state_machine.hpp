#pragma once
#include "ST-LIB.hpp"
#include "TCU_leds/TCU_leds.hpp"
#include "TCU_pressure_sensor/TCU_pressure_sensor.hpp"
using namespace std::chrono_literals;


namespace state_machine{

#define IDEAL_PRESSURE 1.0
#define MIN_PRESSURE_PUMP 0.9
#define MAX_PRESSURE_PUMP 1.1
#define MIN_PRESSURE_FAULT 0.8
#define MAX_PRESSURE_FAULT 1.2

//######################-----variables-----#############################
StateMachine PrincipalStateMachine;
StateMachine PumpStateMachine;

bool ended_setup = false;

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

bool initial_operational_check(){
	return ended_setup;
}

bool operational_fault_check(){
	double pressure = pressure_sensor::get_pressure();
	return pressure < MIN_PRESSURE_FAULT || pressure > MAX_PRESSURE_FAULT;
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
	pressure_sensor::check_pressure();
	leds::operational_led.turn_on();
}

void entry_fault(){
	leds::fault_led.turn_on();
}


//########################-----init methods-----#############################


void add_transitions(){
	PrincipalStateMachine.add_transition(OPERATIONAL,FAULT,operational_fault_check);
	PrincipalStateMachine.add_transition(INITIAL,OPERATIONAL,initial_operational_check);
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
	PrincipalStateMachine.add_low_precision_cyclic_action(pressure_sensor::check_pressure, 10ms,OPERATIONAL);
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
}

void update(){
	PrincipalStateMachine.check_transitions();
}

}
