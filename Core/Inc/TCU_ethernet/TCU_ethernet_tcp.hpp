#pragma once
#include "TCU_ethernet/TCU_ethernet_common.hpp"
#include "TCU_state_machine/TCU_state_machine.hpp"
#include "TCU_common/TCU_common.hpp"

using namespace common;

namespace ethernet{

#define TCP_CONNECTION_TIMEOUT_MILLISECONDS 30000

ServerSocket server_connection;

void start_pumping();
void stop_pumping();
void emergency_state(){
	common::send_to_fault();
}

//StackOrder<0> start_pumping_order;
//StackOrder<0> stop_pumping_order;
StackOrder<0> *emergency_state_order;

void init(){
	emergency_state_order = new StackOrder<0>(EMERGENCY_STATE_PACKET_ID,emergency_state);
	server_connection = ServerSocket(TCU_IP,TCP_SERVER_PORT);
}

bool is_connected(){
	return server_connection.state == ServerSocket::ServerState::ACCEPTED;
}

}
