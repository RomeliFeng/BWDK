/*
 * U_ADC1.cpp
 *
 *  Created on: 2016��11��23��
 *      Author: Romeli
 */

#include "U_ADC1.h"

U_ADC1Class U_ADC1;

uint16_t U_ADC1Data = 0;

void U_ADC1Class::Init() {
	ADCInit();
}

void U_ADC1Class::RegularChannelConfig(uint8_t ADC_Channel,
		uint8_t ADC_SampleTime) {
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime);
}

void U_ADC1Class::RefreshData() {
	ADC1->CR2 |= ((uint32_t) 0x00500000);
	while ((ADC1->SR & ADC_FLAG_EOC) == (uint8_t) RESET)
		;
	U_ADC1Data = ADC1->DR;
}

void U_ADC1Class::RefreshData(uint8_t ADC_Channel, uint8_t ADC_SampleTime) {
	RegularChannelConfig(ADC_Channel, ADC_SampleTime);
	RefreshData();
}

void U_ADC1Class::RefreshData(uint8_t ADC_Channel, uint8_t ADC_SampleTime,
		uint8_t OverLevel) {
	uint32_t sum = 0;
	RegularChannelConfig(ADC_Channel, ADC_SampleTime);
	for (uint16_t i = 0; i < (1 << OverLevel << OverLevel); ++i) {
		RefreshData();
		sum += U_ADC1Data;
	}
	U_ADC1Data = sum >> OverLevel;
}

void U_ADC1Class::ADCInit() {
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);

	ADC_Cmd(ADC1, ENABLE);

	ADC_TempSensorVrefintCmd(ENABLE);

	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1))
		;

	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1))
		;
}
