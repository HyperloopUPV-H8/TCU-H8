#pragma once
#include "ST-LIB.hpp"
#include "TCU_common/TCU_supply_out.hpp"

namespace common{


//#######################-----constants-----############################

#define IDEAL_PRESSURE 1.0
#define MIN_PRESSURE_PUMP 0.9
#define MAX_PRESSURE_PUMP 1.1
#define MIN_PRESSURE_FAULT 0.8
#define MAX_PRESSURE_FAULT 1.2
#define MIN_TEMPERATURE_FAULT 0.0
#define MAX_TEMPERATURE_FAULT 70.0

#define TCP_CLIENT_PORT 50401
#define TCP_SERVER_PORT 50500
#define UDP_PORT 50400
#define TCP_CONNECTION_TIMEOUT_MILLISECONDS 2000

#define START_PUMPING_PACKET_ID 1400
#define STOP_PUMPING_PACKET_ID 1401
#define EMERGENCY_STATE_PACKET_ID 1402
#define STATE_MACHINE_PACKET_ID 1403
#define PRESSURE_PACKET_ID 1404
#define TEMPERATURE_PACKET_ID 1405

const string TCU_IP_STR = "192.168.0.14";
const string BACKEND_IP_STR = "192.168.0.9";
const IPV4 TCU_IP = {"192.168.0.14"};
const IPV4 BACKEND_IP = {"192.168.0.9"};


//####################-----enum of states-----###########################

enum principal_state_machine_states{
	INITIAL,
	OPERATIONAL,
	FAULT,
};

enum pump_state_machine_states{
	IDLE,
	FIRST_PUMPING,
	READY,
	PUMPING,
};


//###################-----enum of functions-----###########################

//function<void()> send_to_fault = [](){};
void send_to_fault();
principal_state_machine_states get_principal_state();
pump_state_machine_states get_pump_state();
void set_pumping_state(pump_state_machine_states state);

}
