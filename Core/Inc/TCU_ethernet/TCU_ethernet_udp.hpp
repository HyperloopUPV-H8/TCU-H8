#pragma once
#include "TCU_ethernet/TCU_ethernet_common.hpp"


namespace ethernet{

DatagramSocket *backend_connection;
double *pressure;
double *temperature;
uint8_t *state;
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
	StackPacket<8,double> pressure_packet(PRESSURE_PACKET_ID,pressure);
	backend_connection->send(pressure_packet);
	*temperature = pressure_sensor::get_temperature();
	StackPacket<8,double> temperature_packet(TEMPERATURE_PACKET_ID,temperature);
	backend_connection->send(temperature_packet);
	pending_communication = false;
}

}
