/*
 * U_USART1.cpp
 *
 *  Created on: 2016��1��18��
 *      Author: Romeli
 */
#include "U_USART1.h"

#include <stdlib.h>
#include <stdio.h>

SerialClass Serial;

extern void Serial_Event();

typedef struct _SendBuf_Typedef {
	uint8_t data[USART1_TX_Buf_Size]; //data
	uint16_t sp = 0; //data sp
	bool busy = false; //busy flag
} SendBuf_Typedef;

typedef struct _ReceiveBuf_Typedef {
	uint8_t data[USART1_RX_Buf_Size]; //data
	uint16_t sp = 0; //data sp
	uint16_t read_sp = 0; //read sp
	bool frame = false; //new frame received flag
} ReceiveBuf_Typedef;

volatile static SendBuf_Typedef Tx_Buf;
volatile static ReceiveBuf_Typedef Rx_Buf;

#ifdef USE_DMA
volatile static SendBuf_Typedef Tx_Buf2;
volatile static bool DMA_Tx_Busy = false;
volatile static uint8_t DMA_Tx_Ch = 1; //1:Tx_Buf.data;2:Tx_Buf2.data
volatile static uint8_t DMA_Tx_Buf[USART1_RX_Buf_Size];
#endif

void SerialClass::begin(uint32_t BaudRate, uint16_t USART_Parity) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//开启USART1和GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,
			ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	//设置PA9复用输出模式（TX）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//设置PA10浮空输入模式（RX）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//设置流控制引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_8);

	//配置USART1 全双工 停止位1 无校验
	USART_DeInit(USART1);
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	if (USART_Parity == USART_Parity_No) {
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	} else {
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	}

	USART_Init(USART1, &USART_InitStructure);

#ifdef USE_DMA
	DMA_InitTypeDef DMA_InitStructure;

	//开启DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&USART1->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) Tx_Buf.data; //临时设置，无效
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 10; //临时设置，无效
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	//发送DMA不开启
//	DMA_Cmd(DMA1_Channel4, ENABLE);

	DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&USART1->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) DMA_Tx_Buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = USART1_RX_Buf_Size;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel5, ENABLE);

	//配置DMA中断
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	//串口发送接收的DMA功能
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
#endif

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;

	NVIC_Init(&NVIC_InitStructure);

#ifndef USE_DMA
	//开启串口的字节接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif
	//开启串口的帧接收中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

	USART_Cmd(USART1, ENABLE);
}

void SerialClass::print(long data, uint8_t base) {
	uint8_t str[20];
	uint8_t len = byNumber(data, base, str);
	print(str, len);
}
void SerialClass::print(double data, uint8_t ndigit) {
	uint8_t str[20];
	uint8_t len = byFloat(data, ndigit, str);
	print(str, len);
}

void SerialClass::print(uint8_t *data, uint16_t len) {
#ifdef USE_DMA
	//判断当前是否正在发送
	if (DMA_Tx_Busy) {
		//根据正在使用的缓冲区转换到空闲缓冲区
		switch (DMA_Tx_Ch) {
		case 1:
			//缓冲区1正在使用中，填充缓冲区2
			//置位缓冲区2忙标志，防止其他中断误修改
			SWCH_1: Tx_Buf2.busy = true;
			while (len--) {
				//逐字节搬运数据
				Tx_Buf2.data[Tx_Buf2.sp++] = *data++;
				//判断缓冲区2是否已满（需要避免此状况）
				if (Tx_Buf2.sp == USART1_TX_Buf_Size) {
					//缓冲区2已满，关闭缓冲区2忙标志
					Tx_Buf2.busy = false;
					//判断是否正在发送
					if (!DMA_Tx_Busy) {
						//发送队列空闲，先将满的缓冲区2发送出去
						DMASend(2);
					} else {
						//发送中，等待发送完成
						while (DMA_Tx_Ch != 2)
							;
					}
					//跳转到缓冲区1继续填充
					goto SWCH_2;
				}
			}
			//缓冲区2填充完成，关闭缓冲区2忙标志
			Tx_Buf2.busy = false;
			//如果发送忙标志未置位，立即发送缓冲区2的数据
			if (!DMA_Tx_Busy) {
				//使用缓冲区2发送数据
				DMASend(2);
			}
			//若发送队列忙，将在发送完当前队列后自动载入
			break;
		case 2:
			//缓冲区2正在使用中，填充缓冲区1
			//置位缓冲区1忙标志，防止其他中断误修改
			SWCH_2: Tx_Buf.busy = true;
			while (len--) {
				//逐字节搬运数据
				Tx_Buf.data[Tx_Buf.sp++] = *data++;
				//判断缓冲区1是否已满（需要避免此状况）
				if (Tx_Buf.sp == USART1_TX_Buf_Size) {
					//缓冲区1已满，关闭缓冲区1忙标志
					Tx_Buf.busy = false;
					//判断是否正在发送
					if (!DMA_Tx_Busy) {
						//发送队列空闲，先将满的缓冲区1发送出去
						DMASend(1);
					} else {
						//发送中，等待发送完成
						while (DMA_Tx_Ch != 1)
							;
					}
					//跳转到缓冲区2继续填充
					goto SWCH_1;
				}
			}
			//缓冲区1填充完成，关闭缓冲区2忙标志
			Tx_Buf.busy = false;
			//如果发送忙标志未置位，立即发送缓冲区2的数据
			if (!DMA_Tx_Busy) {
				DMASend(1);
			}
			//若发送队列忙，将在发送完当前队列后自动载入
			break;
		default:
			break;
		}
	} else {
		//DMA空闲，直接使用缓冲区1发送数据
		while (len--) {
			Tx_Buf.data[Tx_Buf.sp++] = *data++;
		}
		DMASend(1);
	}
#else
	while (len--) {
		write(*data++);
	}
#endif
}

#ifdef USE_DMA
void SerialClass::DMASend(uint8_t ch) {
	volatile uint8_t *TX_Buf_Add;
	uint16_t TX_Len;
	switch (ch) {
	case 1:
		TX_Buf_Add = Tx_Buf.data;
		TX_Len = Tx_Buf.sp;
		break;
	case 2:
		TX_Buf_Add = Tx_Buf2.data;
		TX_Len = Tx_Buf2.sp;
		break;
	default:
		TX_Buf_Add = Tx_Buf.data;
		TX_Len = Tx_Buf.sp;
		break;
	}
	DMA_Tx_Ch = ch;
	DMA_Tx_Busy = true;
	DMA1_Channel4->CMAR = (uint32_t) TX_Buf_Add;
	DMA1_Channel4->CNDTR = TX_Len;

	//使能发送
	GPIO_SetBits(GPIOA, GPIO_Pin_8);

	DMA_Cmd(DMA1_Channel4, ENABLE);
}
#endif

//串口发送一个字节
void SerialClass::write(uint8_t c) {
#ifdef USE_DMA
	print(&c, 1);
#else
	USART1->DR = (c & (uint16_t) 0x01FF);
	while (!(USART1->SR & USART_FLAG_TXE))
	;
#endif
}

//读取一个字节数据，不移动指针
uint8_t SerialClass::peek() {
	if (Rx_Buf.read_sp == Rx_Buf.sp)
		return -1;
	else
		return Rx_Buf.data[Rx_Buf.read_sp];
}

uint8_t SerialClass::peekNextDigit(bool detectDecimal) {
	char data = peek();
	if (data == '-') {
		ReadSPInc();
		return data;
	} else if (data >= '0' && data <= '9') {	//is digit
		ReadSPInc();
		return data;
	} else if (data == '.' && detectDecimal) {
		ReadSPInc();
		return data;
	} else
		return -1;
}

uint8_t SerialClass::read() {
	uint8_t data = peek();
	ReadSPInc();
	return data;
}

void SerialClass::read(uint8_t *buf, uint16_t len) {
	while (len--) {
		*buf++ = read();
	}
}

long SerialClass::nextInt() {
	long data = 0;
	bool isNeg = false;
	char c;
	while (available() > 0) {
		c = peekNextDigit();
		if (c == -1) {
			break;
		} else if (c == '-') {
			if (isNeg) {
				ReadSPDec();
				break;
			}

			else {
				isNeg = true;
				continue;
			}
		}
		data = data * 10 + c - '0';
	}
	return isNeg ? -data : data;
}

double SerialClass::nextFloat() {
	double data = 0.0;
	double frac = 1.0;
	bool isNeg = false;
	bool isFra = false;
	char c;
	while (available() > 0) {
		c = peekNextDigit(true);
		if (c == -1) {
			break;
		} else if (c == '-') {
			if (isNeg) {
				ReadSPDec();
				break;
			}

			else {
				isNeg = true;
				continue;
			}
		} else if (c == '.') {
			if (isFra) {
				ReadSPDec();
				break;
			} else {
				isFra = true;
				continue;
			}
		}
		if (isFra) {
			frac *= 0.1;
		}
		data = data * 10 + c - '0';
	}
	data = isNeg ? -data : data;
	data = isFra ? data * frac : data;
	return data;
}

//计算并返回串口缓冲中剩余未读字节
uint16_t SerialClass::available() {
	return Rx_Buf.sp >= Rx_Buf.read_sp ? Rx_Buf.sp - Rx_Buf.read_sp :
	USART1_RX_Buf_Size - Rx_Buf.read_sp + Rx_Buf.sp;
}

//判断帧接收标志
bool SerialClass::checkFrame() {
	if (Rx_Buf.frame != 0) {
		Rx_Buf.frame = 0;
		return true;
	} else
		return false;
}

//判断发送忙标志
bool SerialClass::checkBusy() {
	return DMA_Tx_Busy;
}

//将读取指针设置为接收缓冲指针，丢弃之前的数据
void SerialClass::clear() {
	Rx_Buf.read_sp = Rx_Buf.sp;
}

uint16_t SerialClass::getlen(uint8_t* data) {
	uint16_t len = 0;
	while (*data++ != '\0')
		len++;
	return len;
}

//串口读取指针+1
inline void SerialClass::ReadSPInc() {
	if (Rx_Buf.read_sp != Rx_Buf.sp)
		++Rx_Buf.read_sp;
	if (Rx_Buf.read_sp == USART1_RX_Buf_Size) {
		Rx_Buf.read_sp = 0;
	}
}

//串口读取指针-1
inline void SerialClass::ReadSPDec() {
	if (Rx_Buf.read_sp == 0) {
		Rx_Buf.read_sp = USART1_RX_Buf_Size - 1;
	} else {
		--Rx_Buf.read_sp;
	}
}

//串口接收中断
extern "C" void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
		Rx_Buf.frame = 1;
		USART_ReceiveData(USART1);
#ifdef USE_DMA
		//关闭DMA接收
		DMA_Cmd(DMA1_Channel5, DISABLE);
		uint16_t len = USART1_RX_Buf_Size - DMA1_Channel5->CNDTR;
		//清除DMA标志
		DMA_ClearFlag(
		DMA1_FLAG_GL5 | DMA1_FLAG_TC5 | DMA1_FLAG_TE5 | DMA1_FLAG_HT5);
		//复位DMA接收区大小
		DMA1_Channel5->CNDTR = USART1_RX_Buf_Size;
		//循环搬运数据
		for (uint16_t i = 0; i < len; ++i) {
			Rx_Buf.data[Rx_Buf.sp++] = DMA_Tx_Buf[i];
			if (Rx_Buf.sp == USART1_RX_Buf_Size) {
				Rx_Buf.sp = 0;
			}
		}
		//开启DMA接收
		DMA_Cmd(DMA1_Channel5, ENABLE);
#endif
		//串口帧接收事件
		Serial_Event();
	}
#ifndef USE_DMA
	//串口字节接收中断置位
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		//搬运数据到缓冲区
		Rx_Buf.data[Rx_Buf.sp] = USART1->DR;
		Rx_Buf.sp++;
		if (Rx_Buf.sp == USART1_RX_Buf_Size)
		Rx_Buf.sp = 0;
	}
#endif
	//串口帧错误中断
	if ((USART1->SR & USART_FLAG_ORE) != (uint16_t) RESET)
		USART_ReceiveData(USART1);
}

#ifdef USE_DMA
//DMA串口发送完成中断
extern "C" void DMA1_Channel4_IRQHandler(void) {
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_ClearFlag(DMA1_FLAG_TC4);
	//判断当前使用的缓冲通道
	switch (DMA_Tx_Ch) {
	case 1:
		//缓冲区1发送完成，置位指针
		Tx_Buf.sp = 0;
		//判断缓冲区2是否有数据，并且忙标志未置位（防止填充到一半发送出去）
		if (Tx_Buf2.sp != 0 && Tx_Buf2.busy == false) {
			//当前使用缓冲区切换为缓冲区2，并加载DMA发送
			DMA_Tx_Ch = 2;
			DMA1_Channel4->CMAR = (uint32_t) Tx_Buf2.data;
			DMA1_Channel4->CNDTR = Tx_Buf2.sp;
			DMA_Cmd(DMA1_Channel4, ENABLE);
		} else {
			//无数据需要发送，清除发送队列忙标志
			DMA_Tx_Busy = false;
			while (!(USART1->SR & USART_FLAG_TC))
				;
			GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		}
		break;
	case 2:
		//缓冲区2发送完成，置位指针
		Tx_Buf2.sp = 0;
		//判断缓冲区1是否有数据，并且忙标志未置位（防止填充到一半发送出去）
		if (Tx_Buf.sp != 0 && Tx_Buf2.busy == false) {
			//当前使用缓冲区切换为缓冲区1，并加载DMA发送
			DMA_Tx_Ch = 1;
			DMA1_Channel4->CMAR = (uint32_t) Tx_Buf.data;
			DMA1_Channel4->CNDTR = Tx_Buf.sp;
			DMA_Cmd(DMA1_Channel4, ENABLE);
		} else {
			//无数据需要发送，清除发送队列忙标志
			DMA_Tx_Busy = false;
			while (!(USART1->SR & USART_FLAG_TC))
				;
			GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		}
		break;
	default:
		break;
	}
}

#endif

void __attribute__((weak)) Serial_Event() {

}

