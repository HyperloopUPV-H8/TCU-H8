#pragma once
#include "ST-LIB.hpp"

namespace pressure_sensor {



#define MAX_UINT64 18446744073709551615U

#define COMMUNICATION_PROTECTION true
#define I2C_SENSOR_BUSY_MASK 0x20
#define I2C_SENSOR_ID 0x28
#define PRESSURE_SENSOR_MAX_BARS 1.0
#define PRESSURE_SENSOR_MIN_BARS 0.0
#define PRESSURE_SENSOR_MAX_OUTPUT 15099494.0
#define PRESSURE_SENSOR_MIN_OUTPUT 1677722.0
#define SENSOR_SETUP_TIMEOUT_NANOSECONDS 5000000000
#define SENSOR_READ_TIMEOUT_NANOSECONDS 20000000
#define SENSOR_WAIT_BETWEEN_I2C_PACKETS_NANOSECONDS 4000000
#define MAX_CLONED_ARRAY_COUNT 5


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
double temperature_in_degrees = -274.0;
uint8_t last_order_array[] = {0,0,0,0,0,0,0};
uint8_t cloned_order_array_counter = 0;
bool pending_communication = false;
bool failed_communication = false;


inline double get_pressure(){return pressure_in_bars;}
inline double get_temperature(){return temperature_in_degrees;}
inline bool get_communication_fault(){return failed_communication && COMMUNICATION_PROTECTION;}

inline void set_pending_communication(){pending_communication=true;}
inline bool communication_is_pending(){return pending_communication;}


/**
 * @brief	updates pressure_in_bars and and temperature_in_degrees using the value on receive_i2c_order
 */
void calculate_pressure_temperature() {
	pressure_in_bars = ((receive_i2c_order[3] * 1.0 + receive_i2c_order[2] * 256.0 + receive_i2c_order[1] * 65536.0) - PRESSURE_SENSOR_MIN_OUTPUT)
		* (PRESSURE_SENSOR_MAX_BARS - PRESSURE_SENSOR_MIN_BARS) / (PRESSURE_SENSOR_MAX_OUTPUT - PRESSURE_SENSOR_MIN_OUTPUT) + PRESSURE_SENSOR_MIN_BARS;
	temperature_in_degrees = ((receive_i2c_order[6] * 1.0 + receive_i2c_order[5] * 256.0 + receive_i2c_order[4] * 65536.0));
}


/**
 * @brief	works as a delay and checks if the value received as a parameter happened more than
 * 			SENSOR_READ_TIMEOUT_NANOSECONDS ago
 * @param check_start 	Clock_value drawn by Time::get_global_tick() to compare with the actual time
 * @retval 	if check_start happened more than SENSOR_WAIT_BETWEEN_I2C_PACKETS_NANOSECONDS before the actual clock value.
 */
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


/**
 * @brief 	this code checks if the array was the same as the last array, and if that is the case counts the repetitions
 * 			it returns false unless array is repeated MAX_CLONED_ARRAY_COUNT times, in which case returns true to warn check_pressure() about the error
 * 			MADE ONLY TO BE USED ON check_array() METHOD
 * @retval	wheter or not MAX_CLONED_ARRAY_COUNT was reached which means sensor is broken
 */
bool check_array(){
	bool same = true;
	for (uint8_t i = 0; i < receive_i2c_order_size; i++){
		same = same && (receive_i2c_order[i] == last_order_array[i]);
	}
	if(same){
		cloned_order_array_counter++;
	}
	if(cloned_order_array_counter > MAX_CLONED_ARRAY_COUNT){
		printf("Received too many copied values from TCU sensors, sensor may be broken. Going into FAULT");
		return true;
	}
	return false;
}


/**
 * @brief 	This method tries to obtain the pressure from the sensor and calculates it.
 * 			In case it fails it sets a variable bound to the state machine to true so it can go fault on its next check.
 * 			Cases of failure are:
 * 			Timeout SENSOR_READ_TIMEOUT_NANOSECONDS happens before receiving the packet (normally when channel is not properly set)
 * 			Exactly same information repeated MAX_CLONED_ARRAY_COUNT times (happens when sensor is broken)
 */
bool check_pressure() {
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
	if(completed){
		calculate_pressure_temperature();
	}else{
		printf("wait time until TCU sensors responds with values expired, going into FAULT state \n");
		failed_communication = true;
	}
	pending_communication = false;
	return completed;
}


/**
 * @brief 	handles all inscribes and pre-start actions for the communication to work
 */
void inscribe(){
	i2c_handler_id = I2C::inscribe(I2C::i2c2);
}


/**
 * @brief	tries to communicate with the sensor until it succeeds or
 * 			used on TCU::start() so the board stays on INITIAL state until communication is properly set
 * @retval	return wether or not setup_communication was successful
 */
bool setup_communication(){
	uint64_t setup_start = Time::get_global_tick();
	auto check_setup_timeout = [&](){
		uint64_t now = Time::get_global_tick();
		if(now < setup_start){
			return (MAX_UINT64 - setup_start + now > SENSOR_SETUP_TIMEOUT_NANOSECONDS);
		}else{
			return (now - setup_start > SENSOR_SETUP_TIMEOUT_NANOSECONDS);
		}
		return true;
	};
	while(!check_setup_timeout()){
		if(check_pressure()){
			failed_communication = false;
			return true;
		}
	}
	printf("setup failed as the TCU couldn t establish communication with the TCU sensors, going into FAULT state");
	return false;
}


}
