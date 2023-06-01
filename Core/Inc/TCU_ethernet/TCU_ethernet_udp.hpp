#pragma once
#include "ST-LIB.hpp"
#include "TCU_pressure_sensor/TCU_pressure_sensor.hpp"

namespace ethernet{

#define TCU_IP "192.168.1.4"
#define BACKEND_IP "192.168.1.3"
#define UDP_PORT 50400

#define PRESSURE_PACKET_ID 303
#define TEMPERATURE_PACKET_ID 304

DatagramSocket *backend_connection;
double *pressure;
double *temperature;

void start_datagram_socket(){
	backend_connection = new DatagramSocket(TCU_IP,UDP_PORT,BACKEND_IP,UDP_PORT);
}

void send_to_backend(Packet& packet){
	backend_connection->send(packet);
}

void send_sensor_variables(){
	*pressure = pressure_sensor::get_pressure();
	StackPacket<8,double> pressure_packet(PRESSURE_PACKET_ID,pressure);
	backend_connection->send(pressure_packet);
	*temperature = pressure_sensor::get_temperature();
	StackPacket<8,double> temperature_packet(TEMPERATURE_PACKET_ID,temperature);
	backend_connection->send(temperature_packet);
}

}
