#ifndef ILI9163_H
#define ILI9163_H

#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_spi.h>

#define P_CS	GPIO_Pin_11
#define P_AO	GPIO_Pin_12
#define G_CS	GPIOB
#define G_AO	GPIOB

typedef enum{
	Reg, Dat
} RegData;

void ILI_Init(void);

#endif
