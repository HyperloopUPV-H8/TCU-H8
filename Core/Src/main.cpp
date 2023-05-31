#include "main.h"
#include "lwip.h"

#include "ST-LIB.hpp"
#include "Runes/Runes.hpp"
#include "TCU.hpp"

int main(void)
{
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


