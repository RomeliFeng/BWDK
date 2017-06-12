#include <TM16xx.h>

#define STB_SET GPIOPort->BSRR = STB_PIN
#define STB_RESET GPIOPort->BRR = STB_PIN
#define CLK_SET GPIOPort->BSRR = CLK_PIN
#define CLK_RESET GPIOPort->BRR = CLK_PIN
#define DIO_SET GPIOPort->BSRR = DIO_PIN
#define DIO_RESET GPIOPort->BRR = DIO_PIN

#define DIO_READ (GPIOPort->IDR & DIO_PIN)

TM16xxClass TM1668 = TM16xxClass(GPIOB, GPIO_Pin_3, GPIO_Pin_8, GPIO_Pin_9);

TM16xxClass::TM16xxClass(GPIO_TypeDef* gpioPort, uint16_t stb_pin,
		uint16_t clk_pin, uint16_t dio_pin) {
	GPIOPort = gpioPort;
	STB_PIN = stb_pin;
	CLK_PIN = clk_pin;
	DIO_PIN = dio_pin;
}

void TM16xxClass::Init() {
	GPIOInit();

	STB_SET;
	CLK_SET;
	DIO_SET;
}

void TM16xxClass::RefreshData() {
	//设置显示模式
	STB_RESET;
	Delay();
	Write(TM16xxCommond_DisplayMode_7x11);
	STB_SET;
	Delay();
	//设置数据命令
	STB_RESET;
	Delay();
	Write(TM16xxCommond_Data_Write_Inc);
	STB_SET;
	Delay();
	//设置地址并填充数据
	STB_RESET;
	Delay();
	Write(TM16xxCommond_Add_00H);
	for (uint8_t i = 0; i < 14; ++i) {
		Write(Data[i]);
	}
	STB_SET;
	Delay();
	//设置显示控制
	STB_RESET;
	Delay();
	Write(TM16xxCommond_DisplayControl_On_14_16);
	STB_SET;
	Delay();

	//设置读按键指令
	STB_RESET;
	Delay();
	Write(TM16xxCommond_Data_Read_Inc);
	//开始读取按键数据
	for (uint8_t i = 0; i < 5; ++i) {
		Key[i].byte = Read();
	}

	STB_SET;
	Delay();

}

void TM16xxClass::Write(uint8_t data) {
	for (uint8_t mask = 0x01; mask != 0; mask <<= 1) {
		CLK_RESET;
		if ((data & mask) != 0) {
			DIO_SET;
		} else {
			DIO_RESET;
		}
		Delay();
		CLK_SET;
		Delay();
	}
}

uint8_t TM16xxClass::Read() {
	uint8_t data = 0;
	DIO_SET;
	for (uint8_t mask = 0x01; mask != 0; mask <<= 1) {
		CLK_RESET;
		Delay();
		CLK_SET;
		Delay();
		if (DIO_READ != 0) {
			data |= mask;
		} else {
			data &= (~mask);
		}
	}
	return data;
}

void TM16xxClass::GPIOInit() {
	GPIO_InitTypeDef GPIO_InitStructure;
	if (GPIOPort == GPIOA) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	} else if (GPIOPort == GPIOB) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	} else if (GPIOPort == GPIOC) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	} else if (GPIOPort == GPIOD) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	} else if (GPIOPort == GPIOE) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	} else if (GPIOPort == GPIOF) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	}

//特殊需求 使用了JTAG引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_InitStructure.GPIO_Pin = STB_PIN | CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOPort, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = DIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOPort, &GPIO_InitStructure);
}

void TM16xxClass::Close() {
	STB_RESET;
	Delay();
	Write(TM16xxCommond_DisplayControl_Off);
	STB_SET;
}

void TM16xxClass::Delay() {
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
	volatile uint16_t j;
#pragma GCC diagnostic pop
	uint16_t i = 5;
	while (i--)
		;
	j = i;
}
