#pragma once
#include "ST-LIB.hpp"
#include "IPV4/IPV4.hpp"
#include "TCU_pressure_sensor/TCU_pressure_sensor.hpp"

namespace ethernet{

#define TCP_CLIENT_PORT 50401
#define TCP_SERVER_PORT 50500
#define UDP_PORT 50400

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


}
