#include <ILI9163.h>

GPIO_InitTypeDef G;
SPI_InitTypeDef S;

void SB(uint8_t data, RegData rd){
	GPIO_ResetBits(G_CS, P_CS);

	if(rd==Reg) GPIO_ResetBits(G_AO, P_AO);
	else GPIO_SetBits(G_AO, P_AO);

	SPI_SendData8(SPI2, data);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	GPIO_SetBits(G_CS, P_CS);
}

void SW(uint16_t data, RegData rd){
	GPIO_ResetBits(G_CS, P_CS);

	if(rd==Reg) GPIO_ResetBits(G_AO, P_AO);
	else GPIO_SetBits(G_AO, P_AO);

	SPI_SendData8(SPI2, data>>8);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	SPI_SendData8(SPI2, data&255);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	GPIO_SetBits(G_CS, P_CS);
}

void SetAddr(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){
	SB(0x2A, Reg);
	SB(0x00, Dat);
	SB(x1, Dat);
	SB(0x00, Dat);
	SB(x2, Dat);

	SB(0x2B, Reg);
	SB(0x00, Dat);
	SB(y1, Dat);
	SB(0x00, Dat);
	SB(y2, Dat);

	SB(0x2C, Reg);
}

void ILI_Init(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	G.GPIO_Pin = P_AO;
	G.GPIO_Mode = GPIO_Mode_OUT;
	G.GPIO_OType = GPIO_OType_PP;
	G.GPIO_PuPd = GPIO_PuPd_NOPULL;
	G.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_Init(G_AO, &G);

	G.GPIO_Pin = P_CS;
	GPIO_Init(G_CS, &G);

	G.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	G.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOB, &G);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_0);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_0);

	S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	S.SPI_CPHA = SPI_CPHA_1Edge;
	S.SPI_CPOL = SPI_CPOL_Low;
	S.SPI_DataSize = SPI_DataSize_8b;
	S.SPI_Direction = SPI_Direction_1Line_Tx;
	S.SPI_FirstBit = SPI_FirstBit_MSB;
	S.SPI_Mode = SPI_Mode_Master;
	S.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI2, &S);
	SPI_Cmd(SPI2, ENABLE);


	SB(0x01, Reg); //Software reset
	Delay(10);
	SB(0x11, Reg); //Exit Sleep
	Delay(20);

	SB(0x26, Reg); //Set default gamma
	SB(0x01, Dat);

	SB(0xC0, Reg); //Set Power Control 1
	SB(0x10, Dat);
	SB(0x04, Dat);

	SB(0xC1, Reg); //Set Power Control 2
	SB(0x04, Dat);
	//AVDD: 2xVCI1
	//VCL: -VCI1
	//VGH: 5xVCI1
	//VGL: 5xVCI1
	//VGH-VGL<=32v!

	SB(0xC2, Reg); //Set Power Control 3
	SB(0x03, Dat);
	SB(0x04, Dat);

	SB(0xC3, Reg); //Set Power Control 4 (Idle mode)
	SB(0x00, Dat);
	SB(0x07, Dat);

	SB(0xC5, Reg); //Set VCom Control 1
	SB(0x24, Dat); // VComH = 3v
	SB(0x7B, Dat); // VComL = -1v

	SB(0x13, Reg); //Normal mode on

	SB(0x3A, Reg); //Set pixel mode
	SB(0x05, Dat);

	SB(0x29, Reg); //Display on
	SB(0x36, Reg); //Set Memory access mode
	SB(0x60|(0<<3), Dat);
}

