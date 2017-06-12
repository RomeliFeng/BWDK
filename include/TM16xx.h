/*
 * STB----
 * */
#ifndef TM1620_H
#define TM1620_H

#include "cmsis_device.h"

typedef enum {
	TM16xxCommond_DisplayMode_6x8 = 0x02,
	TM16xxCommond_DisplayMode_7x11 = 0x03,
	TM16xxCommond_Data_Write_Inc = 0x40,
	TM16xxCommond_Data_Write_Const = 0x44,
	TM16xxCommond_Data_Read_Inc = 0x42,
	TM16xxCommond_Data_Read_Const = 0x46,
	TM16xxCommond_DisplayControl_On_1_16 = 0x88,
	TM16xxCommond_DisplayControl_On_2_16 = 0x89,
	TM16xxCommond_DisplayControl_On_4_16 = 0x8a,
	TM16xxCommond_DisplayControl_On_10_16 = 0x8b,
	TM16xxCommond_DisplayControl_On_11_16 = 0x8c,
	TM16xxCommond_DisplayControl_On_12_16 = 0x8d,
	TM16xxCommond_DisplayControl_On_13_16 = 0x8e,
	TM16xxCommond_DisplayControl_On_14_16 = 0x8f,
	TM16xxCommond_DisplayControl_Off = 0x80,
	TM16xxCommond_Add_00H = 0xc0,
	TM16xxCommond_Add_01H = 0xc1,
	TM16xxCommond_Add_02H = 0xc2,
	TM16xxCommond_Add_03H = 0xc3,
	TM16xxCommond_Add_04H = 0xc4,
	TM16xxCommond_Add_05H = 0xc5,
	TM16xxCommond_Add_06H = 0xc6,
	TM16xxCommond_Add_07H = 0xc7,
	TM16xxCommond_Add_08H = 0xc8,
	TM16xxCommond_Add_09H = 0xc9,
	TM16xxCommond_Add_0aH = 0xca,
	TM16xxCommond_Add_0bH = 0xcb,
	TM16xxCommond_Add_0cH = 0xcc,
	TM16xxCommond_Add_0dH = 0xcd
} TM16xxCommond_TypeDef;

class TM16xxClass {
public:
	void Init();
	uint8_t Data[14];
	BytetoBit_Typedef Key[5];
	TM16xxClass(GPIO_TypeDef* gpioPort, uint16_t stb_pin, uint16_t clk_pin,
			uint16_t dio_pin);
	void RefreshData();
	void Close();
	void Open();
private:
	GPIO_TypeDef* GPIOPort;
	uint16_t STB_PIN;
	uint16_t CLK_PIN;
	uint16_t DIO_PIN;

	void GPIOInit();
	void Write(uint8_t data);
	uint8_t Read();
	void Delay();
};

extern TM16xxClass TM1668;
#endif
