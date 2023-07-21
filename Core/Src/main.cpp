#include "main.h"
#include "lwip.h"

#include "ST-LIB.hpp"
#include "Runes/Runes.hpp"
#include "TCU.hpp"

int main(void)
{
	if (not __HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) HAL_NVIC_SystemReset();
		static_assert(HSE_VALUE == 25000000UL);
	#ifndef BOARD
		static_assert(false, "BOARD has to be declared as a symbol");
	#endif

	TCU::start();

	while(1) {
		TCU::update();
	}
}

void Error_Handler(void)
{
	//ErrorHandler("HAL error handler triggered");
	//while (1){}
}



