#pragma once
#include "ST-LIB.hpp"
#include "TCU_common/TCU_common.hpp"

namespace pressure_sensor {



#define MAX_UINT64 18446744073709551615U

#define COMMUNICATION_PROTECTION true
#define I2C_SENSOR_BUSY_MASK 0x20
#define I2C_SENSOR_ID 0x28
#define PRESSURE_SENSOR_MAX_BARS 1.0
#define PRESSURE_SENSOR_MIN_BARS 0.0
#define TEMPERATURE_SENSOR_MAX_DEGREES 150.0
#define TEMPERATURE_SENSOR_MIN_DEGREES -50.0
#define SENSOR_MAX_OUTPUT 15099494.0
#define SENSOR_MIN_OUTPUT 1677722.0
#define TEMPERATURE_SENSOR_SCALER 16777215
#define SENSOR_SETUP_TIMEOUT_MILLISECONDS 5000000
#define SENSOR_READ_TIMEOUT_MILLISECONDS 50
#define SENSOR_PACKET_DELAY_MILLISECONDS 5
#define SENSOR_PERIOD_BETWEEN_READS_MILLISECONDS 500
#define MAX_CLONED_ARRAY_COUNT 5


enum check_sensor_states{
	STARTING,
	SENDING_ORDER,
	WAITING_FLAG,
	READING_SENSOR,
	FINISHED,
	RESET,
};

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

uint64_t sensor_packet_number = 0;
uint8_t check_sensor_state = STARTING;
uint8_t timeout_handler_id = 0;
bool pending_communication = false;
bool packet_ready = false;
bool timed_out = false;
bool failed_communication = false;
bool two_consecutive_limit_measures = false;
bool second_check = false;


double get_pressure(){return pressure_in_bars;}
double get_temperature(){return temperature_in_degrees;}
bool get_communication_fault(){return failed_communication && COMMUNICATION_PROTECTION;}

void set_pending_communication(){pending_communication=true;}
void set_packet_ready(){packet_ready = true;}
void set_timed_out(){timed_out = true;}
void set_two_consecutive_limit_measures(){two_consecutive_limit_measures = true;}
bool communication_is_pending(){return pending_communication;}


/**
 * @brief	updates pressure_in_bars and and temperature_in_degrees using the value on receive_i2c_order
 */
void calculate_pressure_temperature() {
	pressure_in_bars = ((receive_i2c_order[3] * 1.0 + receive_i2c_order[2] * 256.0 + receive_i2c_order[1] * 65536.0) - SENSOR_MIN_OUTPUT)
		* (PRESSURE_SENSOR_MAX_BARS - PRESSURE_SENSOR_MIN_BARS) / (SENSOR_MAX_OUTPUT - SENSOR_MIN_OUTPUT) + PRESSURE_SENSOR_MIN_BARS;
	temperature_in_degrees = ((receive_i2c_order[6] * 1.0 + receive_i2c_order[5] * 256.0 + receive_i2c_order[4] * 65536.0) *
			(TEMPERATURE_SENSOR_MAX_DEGREES - TEMPERATURE_SENSOR_MIN_DEGREES) / TEMPERATURE_SENSOR_SCALER ) + TEMPERATURE_SENSOR_MIN_DEGREES;
}

/**
 * @brief 	check if pressure goes over max or under min safe values
 *
 * @retval	returns true if pressure goes over or under limits
 */
bool check_pressure_limits(){
	return two_consecutive_limit_measures || pressure_in_bars < MIN_PRESSURE_FAULT || pressure_in_bars > MAX_PRESSURE_FAULT;
}

bool check_temperature_limits(){
	return two_consecutive_limit_measures || temperature_in_degrees < MIN_TEMPERATURE_FAULT || temperature_in_degrees > MAX_TEMPERATURE_FAULT;
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
void check_sensor(){
	if(!pending_communication){return;}
	if(check_sensor_state == STARTING){
		//timeout_handler_id = Time::register_low_precision_alarm(SENSOR_READ_TIMEOUT_MILLISECONDS, [&](){set_timed_out();printf("timed out trying to communicate with TCU sensors \n");});
		uint64_t counter = sensor_packet_number;
		Time::set_timeout(SENSOR_READ_TIMEOUT_MILLISECONDS, [&,counter](){
			if(counter == sensor_packet_number){
				set_timed_out();
				printf("timed out trying to communicate with TCU sensors \n");
			}});
		check_sensor_state = SENDING_ORDER;
	}
	if(check_sensor_state == RESET){
		HAL_I2C_DeInit(I2C::active_i2c[i2c_handler_id]->hi2c);
		HAL_I2C_Init(I2C::active_i2c[i2c_handler_id]->hi2c);

		check_sensor_state = SENDING_ORDER;
	}

	if(packet_ready){
		switch(check_sensor_state){
			case SENDING_ORDER:
				if(I2C::transmit_next_packet_polling(i2c_handler_id,*check_order_packet)){
					check_sensor_state = WAITING_FLAG;
					}else{
						check_sensor_state = RESET;
					}
			break;
			case WAITING_FLAG:
				if (I2C::receive_next_packet_polling(i2c_handler_id,*check_ready_packet)) {
					receive_i2c_order[0] = *check_ready_packet->get_data();
					if ((receive_i2c_order[0] & I2C_SENSOR_BUSY_MASK) == 0) {
						check_sensor_state = READING_SENSOR;
						}
					}else{
						check_sensor_state = RESET;
					}

			break;
			case READING_SENSOR:
				if (I2C::receive_next_packet_polling(i2c_handler_id,*receive_order_packet)) {
					receive_i2c_order = receive_order_packet->get_data();
					calculate_pressure_temperature();
					check_sensor_state = FINISHED;
				}else{
					check_sensor_state = RESET;
				}
			break;
			case FINISHED:
				if(check_array()){
					printf("Received too many copied values from the sensor, its not making new readings. Entering FAULT state \n");
					failed_communication = true;
					//Time::unregister_low_precision_alarm(timeout_handler_id);
					return;
				}
				if(check_pressure_limits() || check_temperature_limits()){
					if(second_check){
						set_two_consecutive_limit_measures();
					}
					else{
						second_check = true;
						check_sensor_state = SENDING_ORDER;
					}
				}
				pending_communication = false;
				second_check = false;
				//Time::unregister_low_precision_alarm(timeout_handler_id);
				sensor_packet_number++;
				check_sensor_state = STARTING;
			break;
		}
		packet_ready = false;
		if(timed_out && check_sensor_state != FINISHED){
			printf("wait time until TCU sensors responds with values expired. Entering FAULT state \n");
			failed_communication = true;
		}
	}
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
void setup_communication(){
	check_sensor_state = SENDING_ORDER;
	pending_communication = true;
	uint8_t timeout_alarm = Time::register_low_precision_alarm(SENSOR_SETUP_TIMEOUT_MILLISECONDS, [&](){set_timed_out();printf("failed to communicate with TCU sensor in start up \n");});
	uint8_t check_sensor_alarm = Time::register_low_precision_alarm(SENSOR_PACKET_DELAY_MILLISECONDS, [&](){
		packet_ready = true;

	});
	while(check_sensor_state != FINISHED){
		check_sensor();
		if(timed_out){
			break;
		}
	}
	Time::unregister_low_precision_alarm(timeout_alarm);
	Time::unregister_low_precision_alarm(check_sensor_alarm);
}


}
