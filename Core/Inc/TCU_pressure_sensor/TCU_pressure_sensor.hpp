#pragma once
#include "ST-LIB.hpp"

namespace pressure_sensor {




#define MAX_UINT64 18446744073709551615U

#define I2C_SENSOR_BUSY_MASK 0x20
#define I2C_SENSOR_ID 0x28
#define PRESSURE_SENSOR_MAX_BARS 1.0
#define PRESSURE_SENSOR_MIN_BARS 0.0
#define PRESSURE_SENSOR_MAX_OUTPUT 15099494.0
#define PRESSURE_SENSOR_MIN_OUTPUT 1677722.0
#define SENSOR_READ_TIMEOUT_NANOSECONDS 40000000 //doesn t work if higher than tim23 time period before overflow
#define SENSOR_WAIT_BETWEEN_I2C_PACKETS_NANOSECONDS 5000000

uint8_t i2c_handler_id = 0;
uint8_t check_i2c_order_size = 3;
uint8_t check_i2c_order_array[] = {0xAA,0x00,0x00};
uint8_t *check_i2c_order = check_i2c_order_array;
I2CPacket *check_order_packet = new I2CPacket(I2C_SENSOR_ID,check_i2c_order,check_i2c_order_size);
uint8_t receive_i2c_order_size = 7;
uint8_t receive_i2c_order_array[] = {0,0,0,0,0,0,0};
uint8_t *receive_i2c_order = receive_i2c_order_array;
I2CPacket *receive_order_packet = new I2CPacket(I2C_SENSOR_ID,receive_i2c_order,receive_i2c_order_size);
I2CPacket *check_ready_packet = new I2CPacket(I2C_SENSOR_ID,receive_i2c_order_array[0]);
double pressure_in_bars = -1.0;


double get_pressure(){
	return pressure_in_bars;
}

void calculate_pressure() {
	pressure_in_bars = ((receive_i2c_order[3] * 1.0 + receive_i2c_order[2] * 256.0 + receive_i2c_order[1] * 65536.0) - PRESSURE_SENSOR_MIN_OUTPUT)
		* (PRESSURE_SENSOR_MAX_BARS - PRESSURE_SENSOR_MIN_BARS) / (PRESSURE_SENSOR_MAX_OUTPUT - PRESSURE_SENSOR_MIN_OUTPUT) + PRESSURE_SENSOR_MIN_BARS;
}

bool check_pressure_timeout(uint64_t check_start) {
	uint64_t timeout_start = Time::get_global_tick();
	uint64_t now = Time::get_global_tick();
	while(1){
		now = Time::get_global_tick();
		if(now < timeout_start){
			if(MAX_UINT64 - timeout_start + now > SENSOR_WAIT_BETWEEN_I2C_PACKETS_NANOSECONDS)break;
		}else{
			if(now - timeout_start > SENSOR_WAIT_BETWEEN_I2C_PACKETS_NANOSECONDS)break;
		}

	}

	if (now < check_start) {
		return MAX_UINT64 - check_start + now > SENSOR_READ_TIMEOUT_NANOSECONDS;
	} else {
		return now - check_start > SENSOR_READ_TIMEOUT_NANOSECONDS;
	}
}

void check_pressure() {
	bool completed = false;
	bool timeout = false;
	bool retry_transmit = false;
	uint64_t check_start = Time::get_global_tick();

	while (!completed && !timeout) {
		retry_transmit = false;
		if(I2C::transmit_next_packet_polling(i2c_handler_id,*check_order_packet)){
		while (!completed && !timeout && !retry_transmit) {
			if (I2C::receive_next_packet_polling(i2c_handler_id,*check_ready_packet)) {
				receive_i2c_order[0] = *check_ready_packet->get_data();
				if ((receive_i2c_order[0] & I2C_SENSOR_BUSY_MASK) == 0) {
					if (I2C::receive_next_packet_polling(i2c_handler_id,*receive_order_packet)) {
						receive_i2c_order = receive_order_packet->get_data();
						completed = true;
					} else {
						retry_transmit = true;
					}
				}
			}
			timeout = check_pressure_timeout(check_start);
		}
		}
		timeout = check_pressure_timeout(check_start);
	}
	calculate_pressure();
}

void inscribe(){
	i2c_handler_id = I2C::inscribe(I2C::i2c2);
}

}
