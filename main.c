#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_tim.h>
#include <stm32f0xx_dma.h>
#include <stm32f0xx_spi.h>
#include <Sensor_config.h>
#include <ILI9163.h>

GPIO_InitTypeDef G;
TIM_TimeBaseInitTypeDef TB;
TIM_ICInitTypeDef TI;
TIM_OCInitTypeDef TO;
DMA_InitTypeDef D;
SPI_InitTypeDef S;

//Camera SCB pins
#define C_IGPIO		GPIOC
#define C_SCL		GPIO_Pin_13
#define C_SDA		GPIO_Pin_15

#define C_OVADDR	(0x42)

#define I_DLYL	10
#define I_DLYS	5

//VSync and Href pins
#define P_VSYNC		GPIO_Pin_14
#define G_VSYNC		GPIOC
#define P_HREF		GPIO_Pin_0
#define G_HREF		GPIOC

//SysTick stuffs
volatile uint32_t msec = 0;

void Delay(uint32_t t){
	uint32_t mss = msec;
	while((msec-mss)<t);
}

void SysTick_Handler(void){
	msec++;
}

/*
 *
 * The bit banged I2C is NOT my implementation. I ported it from an STM32 based OV7670 project I found online.
 *
 */
void Delayus(uint32_t D){
	D*=50;
	while(D--) asm volatile("nop");
}

void I_Start(void){
	GPIO_SetBits(C_IGPIO, C_SDA);
	Delayus(I_DLYL);
	GPIO_SetBits(C_IGPIO, C_SCL);
	Delayus(I_DLYL);
	GPIO_ResetBits(C_IGPIO, C_SDA);
	Delayus(I_DLYL);
	GPIO_ResetBits(C_IGPIO, C_SCL);
	Delayus(I_DLYL);
}

void I_Stop(void){
	GPIO_ResetBits(C_IGPIO, C_SDA);
	Delayus(I_DLYL);
	GPIO_SetBits(C_IGPIO, C_SCL);
	Delayus(I_DLYL);
	GPIO_SetBits(C_IGPIO, C_SDA);
	Delayus(I_DLYL);
}

void I_NoAck(void){
	GPIO_SetBits(C_IGPIO, C_SDA);
	Delayus(I_DLYL);
	GPIO_SetBits(C_IGPIO, C_SCL);
	Delayus(I_DLYL);
	GPIO_ResetBits(C_IGPIO, C_SCL);
	Delayus(I_DLYL);
	GPIO_ResetBits(C_IGPIO, C_SDA);
	Delayus(I_DLYL);
}

uint8_t I_WB(uint8_t D){
	uint8_t n, a;

	for(n = 0; n<8; n++){
		if((D<<n)&0x80) GPIO_SetBits(C_IGPIO, C_SDA);
		else GPIO_ResetBits(C_IGPIO, C_SDA);
		Delayus(I_DLYL);
		GPIO_SetBits(C_IGPIO, C_SCL);
		Delayus(I_DLYL);
		GPIO_ResetBits(C_IGPIO, C_SCL);
		Delayus(I_DLYL);
	}

	Delayus(I_DLYS);
	GPIO_SetBits(C_IGPIO, C_SDA);
	Delayus(I_DLYL);
	GPIO_SetBits(C_IGPIO, C_SCL);
	Delayus(I_DLYS);

	if(GPIO_ReadInputDataBit(C_IGPIO, C_SDA)) a = 0;
	else a = 1;
	GPIO_ResetBits(C_IGPIO, C_SCL);
	Delayus(I_DLYL);

	return a;
}

uint8_t I_RB(void){
	uint8_t n, D = 0;

	GPIO_SetBits(C_IGPIO, C_SDA);
	Delayus(I_DLYL);
	for(n = 0; n<8; n++){
		Delayus(I_DLYL);
		GPIO_SetBits(C_IGPIO, C_SCL);
		Delayus(I_DLYL);
		D<<=1;
		if(GPIO_ReadInputDataBit(C_IGPIO, C_SDA)) D++;
		GPIO_ResetBits(C_IGPIO, C_SCL);
		Delayus(I_DLYL);
	}

	return D;
}

uint8_t C_Write(uint8_t Reg, uint8_t Dat){
	I_Start();
	if(!I_WB(C_OVADDR)){
		I_Stop();
		return 0;
	}

	Delayus(I_DLYS);
	if(!I_WB(Reg)){
		I_Stop();
		return 0;
	}

	Delayus(I_DLYS);
	if(!I_WB(Dat)){
		I_Stop();
		return 0;
	}
	I_Stop();

	return 1;
}

uint8_t C_Read(uint8_t Reg, uint8_t *Dat){
	I_Start();
	if(!I_WB(C_OVADDR)){
		I_Stop();
		return 0;
	}

	Delayus(I_DLYS);
	if(!I_WB(Reg)){
		I_Stop();
		return 0;
	}
	I_Stop();

	Delayus(I_DLYS);

	I_Start();
	if(!I_WB(C_OVADDR|1)){
		I_Stop();
		return 0;
	}

	Delayus(I_DLYS);
	*Dat = I_RB();
	I_NoAck();
	I_Stop();

	return 1;
}

uint8_t C_Init(void){
	int32_t n;
	uint8_t V;

	if(!C_Write(0x12, 0x80)) return 0;
	Delay(10);
	if(!C_Read(0x0B, &V)) return 0;

	if(V == 0x73){
		for(n = 0; n<OV7670_REG_NUM; n++){
			if(!C_Write(ovregs[n], ovvals[n])) return 0;
		}
	}

	return 1;
}

int main(void)
{
	//48MHz clock (overclocking can be done by increasing PLLMul! Do at your own risk ;)
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	RCC_PLLCmd(DISABLE);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
	RCC_PLLCmd(ENABLE);
	while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	//Clock enabling...
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	//PA8, TIM1 IC1
	G.GPIO_Pin = GPIO_Pin_8;
	G.GPIO_Mode = GPIO_Mode_AF;
	G.GPIO_OType = GPIO_OType_PP;
	G.GPIO_PuPd = GPIO_PuPd_DOWN;
	G.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_Init(GPIOA, &G);

	//PB2, TIM2 OC2
	G.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &G);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_2);

	//PA0-PA7, OV7670 data output
	G.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	G.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &G);

	//VSYNC
	G.GPIO_Pin = P_VSYNC;
	GPIO_Init(G_VSYNC, &G);

	//HREF
	G.GPIO_Pin = P_HREF;
	GPIO_Init(G_HREF, &G);

	//SCB
	G.GPIO_Pin = C_SCL | C_SDA;
	G.GPIO_OType = GPIO_OType_OD;
	G.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(C_IGPIO, &G);

	//Timer 1 config
	TB.TIM_ClockDivision = TIM_CKD_DIV1;
	TB.TIM_CounterMode = TIM_CounterMode_Up;
	TB.TIM_Period = 0xFF;
	TB.TIM_Prescaler = 0;
	TB.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TB);

	//Timer 2 config
	TB.TIM_ClockDivision = TIM_CKD_DIV1;
	TB.TIM_CounterMode = TIM_CounterMode_Up;
	TB.TIM_Period = 1;
	TB.TIM_Prescaler = 0;
	TB.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TB);

	//Timer 2 toggle output (output 12MHz clock)
	TO.TIM_OCIdleState = TIM_OCIdleState_Set;
	TO.TIM_OCMode = TIM_OCMode_Toggle;
	TO.TIM_OCPolarity = TIM_OCPolarity_High;
	TO.TIM_OutputState = TIM_OutputState_Enable;
	TO.TIM_Pulse = 0;
	TIM_OC2Init(TIM2, &TO);
	TIM_Cmd(TIM2, ENABLE);

	//Timer 1 input capture (DMA trigger)
	TI.TIM_Channel = TIM_Channel_1;
	TI.TIM_ICFilter = 0;
	TI.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TI.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TI.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM1, &TI);
	TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);

	//DMA Config
	D.DMA_BufferSize = 1;
	D.DMA_DIR = DMA_DIR_PeripheralDST;
	D.DMA_M2M = DMA_M2M_Disable;
	D.DMA_MemoryBaseAddr = (uint32_t) &GPIOA->IDR;
	D.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	D.DMA_MemoryInc = DMA_MemoryInc_Disable;
	D.DMA_Mode = DMA_Mode_Circular;
	D.DMA_PeripheralBaseAddr = (uint32_t) &(SPI2->DR);
	D.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	D.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	D.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_Init(DMA1_Channel2, &D);

	//Enable Systick
	SysTick_Config(SystemCoreClock/1000);

	//Camera initialization
	while(!C_Init());

	//LCD initialization
	ILI_Init();

	//Disable Systick
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Pos;

	int x,y;

	uint32_t mso = 0, a = 0, b = 0;
	int row = 0;

	//Set address to bottom of LCD
	SetAddr(0,120,160-1,128-1);

	//Set all pixels in range to zero
	for(y = 0; y<8; y++){
		for(x = 0; x<160; x++){
			SW(0,Dat);
		}
	}


	//Wait for frame blanking!
	while(!GPIO_ReadInputDataBit(G_VSYNC, P_VSYNC));

	//Enable DMA
	DMA_Cmd(DMA1_Channel2, ENABLE);

	//These settings were for entire frame capture and didn't work
	/*SetAddr(0,0,160-1,120-1);
	GPIO_ResetBits(G_CS, P_CS);
	GPIO_SetBits(G_AO, P_AO);*/

	while(1)
	{
		//Wait for blanking to end
		while(GPIO_ReadInputDataBit(G_VSYNC, P_VSYNC));

		//Capture all rows
		for(row = 0; row<120; row++){
			//Ensure SPI isn't busy
			while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
			//Set current LCD memory address range to row
			SetAddr(0,row,160-1,row);

			//Enable LCD and set transfer location to memory (not reg)
			GPIO_ResetBits(G_CS, P_CS);
			GPIO_SetBits(G_AO, P_AO);

			//Wait for line to finish
			while(!GPIO_ReadInputDataBit(G_HREF, P_HREF));
			//Wait for blanking to finish
			while(GPIO_ReadInputDataBit(G_HREF, P_HREF));
		}

		//Wait for frame to finish
		while(!GPIO_ReadInputDataBit(G_VSYNC, P_VSYNC));
	}
}
