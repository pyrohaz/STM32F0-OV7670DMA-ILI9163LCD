/*
 * I found this file zipped up somewhere on the internet in a folder named: OV7670Ã¦â€˜â€žÃ¥Æ’ï¿½Ã¥Â¤Â´STM32Ã©Â©Â±Ã¥Å Â¨Ã¥Â¤Â§Ã©â€ºâ€ Ã¥ï¿½Ë†
 * If I'm not allowed to use this file, please say!
 */

//#define OV7670_REG_NUM  167
#define OV7670_REG_NUM  166

const uint8_t ovregs[] = {
		0x00, 0x3a, 0x40, 0x12, 0x32, 0x17, 0x18, 0x19, 0x1a, 0x03, 0x0c, 0x3e, 0x70, 0x71, 0x72, 0x73, 0xa2,
		0x11, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
		0x89, 0x13, 0x00, 0x10, 0x0d, 0x14, 0xa5, 0xab, 0x24, 0x25, 0x26, 0x9f, 0xa0, 0xa1, 0xa6, 0xa7,
		0xa8, 0xa9, 0xaa, 0x13, 0x0e, 0x0f, 0x16, 0x1e, 0x21, 0x22, 0x29, 0x33, 0x35, 0x37, 0x38, 0x39,
		0x3c, 0x4d, 0x4e, 0x69, 0x6b, 0x74, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x96, 0x9a, 0xb0, 0xb1,
		0xb2, 0xb3, 0xb8, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x64,
		0x65, 0x66, 0x94, 0x95, 0x6c, 0x6d, 0x6e, 0x6f, 0x6a, 0x13, 0x15, 0x4f, 0x50, 0x51, 0x52, 0x53,
		0x54, 0x55, 0x56, 0x57, 0x58, 0x41, 0x3f, 0x75, 0x76, 0x4c, 0x77, 0x3d, 0x4b, 0xc9, 0x41, 0x34,
		0x3b, 0xa4, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x78, 0x79, 0xc8, 0x79, 0xc8,
		0x79, 0xc8, 0x79, 0xc8, 0x79, 0xc8, 0x79, 0xc8, 0x79, 0xc8, 0x79, 0xc8, 0x79, 0xc8, 0x79, 0xc8,
		0x79, 0xc8, 0x69, 0x09, 0x3b, 0x2d, 0x1b};

const uint8_t ovvals[] = {
		0x00, 0x04, 0x10, 0x04, 0xb6, 0x13, 0x01, 0x02, 0x7a, 0x0a, 0x04, 0x1A, 0x3a, 0x35, 0x22, 0xf2, 0x02,
		0x04, 0x20, 0x1c, 0x28, 0x3c, 0x55, 0x68, 0x76, 0x80, 0x88, 0x8f, 0x96, 0xa3, 0xaf, 0xc4, 0xd7,
		0xe8, 0xe7, 0x00, 0x00, 0x00, 0x10, 0x05, 0x07, 0x75, 0x63, 0xA5, 0x78, 0x68, 0x03, 0xdf, 0xdf,
		0xf0, 0x90, 0x94, 0xe5, 0x61, 0x4b, 0x02, 0x07, 0x02, 0x91, 0x07, 0x0b, 0x0b, 0x1d, 0x71, 0x2a,
		0x78, 0x40, 0x20, 0x5d, 0xc0, 0x19, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x84, 0x0c,
		0x0e, 0x82, 0x0a, 0x14, 0xf0, 0x34, 0x58, 0x28, 0x3a, 0x88, 0x88, 0x44, 0x67, 0x49, 0x0e, 0x04,
		0x20, 0x05, 0x04, 0x08, 0x0a, 0x55, 0x11, 0x9f, 0x40, 0xe7, 0x24, 0x80, 0x80, 0x00, 0x22, 0x5e,
		0x80, 0x00, 0x40, 0x90, 0x9e, 0x08, 0x00, 0x05, 0xe1, 0x0F, 0x0a, 0xc2, 0x09, 0x28, 0x38, 0x11,
		0x02, 0x89, 0x00, 0x30, 0x20, 0x30, 0x84, 0x29, 0x03, 0x4c, 0x3f, 0x04, 0x01, 0xf0, 0x0f, 0x00,
		0x10, 0x7e, 0x0a, 0x80, 0x0b, 0x01, 0x0c, 0x0f, 0x0d, 0x20, 0x09, 0x80, 0x02, 0xc0, 0x03, 0x40,
		0x05, 0x30, 0xaa, 0x00, 0x42, 0x01, 0x0F};
