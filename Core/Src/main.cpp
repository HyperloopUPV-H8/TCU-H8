#include "main.h"
#include "lwip.h"

#include "ST-LIB.hpp"
#include "Runes/Runes.hpp"
#include "TCU.hpp"

int main(void)
{
	DigitalOutput errorhandlerled(PB0);
	TCU::start();

	while(1) {
		TCU::update();
		if(ErrorHandlerModel::error_triggered){
			errorhandlerled.turn_on();
		}
	}
}

void Error_Handler(void)
{
	//ErrorHandler("HAL error handler triggered");
	//while (1){}
}



