#pragma once
#include "TCU_ethernet/TCU_ethernet_common.hpp"

using namespace common;

namespace ethernet{

DatagramSocket *backend_connection;
double *pressure = new double;
double *temperature = new double;
uint8_t *state = new uint8_t;
uint8_t *pump_state = new uint8_t;
bool pending_communication = false;


void set_pending_communication(){pending_communication=true;}
bool communication_is_pending(){return pending_communication;}

void start_datagram_socket(){
	backend_connection = new DatagramSocket(TCU_IP,UDP_PORT,BACKEND_IP,UDP_PORT);
}

void send_udp_to_backend(Packet& packet){
	backend_connection->send(packet);
}

void send_sensor_variables(){
	*pressure = pressure_sensor::get_pressure();
	StackPacket pressure_packet(PRESSURE_PACKET_ID,pressure);
	backend_connection->send(pressure_packet);
	*temperature = pressure_sensor::get_temperature();
	StackPacket temperature_packet(TEMPERATURE_PACKET_ID,temperature);
	backend_connection->send(temperature_packet);
	pending_communication = false;
	*state = (uint8_t) common::get_principal_state();
	*pump_state = (uint8_t) common::get_pump_state();
	StackPacket state_packet(STATE_MACHINE_PACKET_ID,state,pump_state);
	backend_connection->send(state_packet);

}

}
