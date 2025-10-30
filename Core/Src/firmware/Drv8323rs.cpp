/*
 * Drv8323rs.cpp
 *
 *  Created on: Oct 30, 2025
 *      Author: jakob
 */

#include "Drv8323rs.hpp"
#include "stm32f0xx_hal.h"

void Drv8323rs::init() {
	HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_SET);
}

bool Drv8323rs::read_fault() {
	return HAL_GPIO_ReadPin(DRV_nFAULT_GPIO_Port, DRV_nFAULT_Pin);
}

void Drv8323rs::reset_fault_hw() {
	HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_RESET);
	// create 20us delay
	uint32_t tick_start = SysTick->VAL;
	uint32_t ticks = 20 * HAL_RCC_GetHCLKFreq() / 1e6;
	while (SysTick->VAL - tick_start < ticks);
	HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_SET);

}

void Drv8323rs::sleep(const bool enable) {
	HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, enable ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
