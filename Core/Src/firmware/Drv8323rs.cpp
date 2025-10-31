/*
 * Drv8323rs.cpp
 *
 *  Created on: Oct 30, 2025
 *      Author: jakob
 */

#include "Drv8323rs.hpp"
#include "stm32f0xx_hal.h"

static constexpr uint16_t ADDR_CONTROL_REGISTER = 0x02;
static constexpr uint16_t ADDR_FAULT_REGISTER_1 = 0x00;
static constexpr uint16_t ADDR_FAULT_REGISTER_2 = 0x01;
static constexpr uint32_t SPI_TIMEOUT = 100;
static SPI_HandleTypeDef hspi;

void Drv8323rs::init(SPI_HandleTypeDef &spi) {
	// Enable Gate Driver
	sleep(false);
	hspi = spi;
}

void Drv8323rs::read_write_spi(uint16_t &rx_buf, uint16_t &tx_buf) {
	// SPI chip select low during communication
	HAL_GPIO_WritePin(DRV_NSS_GPIO_Port, DRV_NSS_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi, reinterpret_cast<uint8_t*>(&tx_buf), reinterpret_cast<uint8_t*>(&rx_buf), 2, SPI_TIMEOUT);
	HAL_GPIO_WritePin(DRV_NSS_GPIO_Port, DRV_NSS_Pin, GPIO_PIN_SET);
}

uint16_t Drv8323rs::read_spi(uint16_t addr) {
	uint16_t tx_buf = (1 << 15) | (addr << 11);
	uint16_t rx_buf{};
	// SPI chip select low during communication
	HAL_GPIO_WritePin(DRV_NSS_GPIO_Port, DRV_NSS_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi, reinterpret_cast<uint8_t*>(&tx_buf), reinterpret_cast<uint8_t*>(&rx_buf), 2, SPI_TIMEOUT);
	HAL_GPIO_WritePin(DRV_NSS_GPIO_Port, DRV_NSS_Pin, GPIO_PIN_SET);
	return rx_buf;
}

Drv8323rs::PwmControlMode Drv8323rs::set_pwm_mode(PwmControlMode new_mode) {
	uint16_t rx_buf;
	uint16_t tx_buf = (0 << 15) | (ADDR_CONTROL_REGISTER << 11) | (new_mode << 5);

	read_write_spi(rx_buf, tx_buf);

	return static_cast<PwmControlMode>(rx_buf >> 5);
}

void Drv8323rs::set_phase_a_high(FetState state) {
	HAL_GPIO_WritePin(DRV_INHA_GPIO_Port, DRV_INHA_Pin, state == FetState::CLOSE ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Drv8323rs::set_phase_a_low(FetState state) {
	HAL_GPIO_WritePin(DRV_INLA_GPIO_Port, DRV_INLA_Pin, state == FetState::CLOSE ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Drv8323rs::set_phase_b_high(FetState state) {
	HAL_GPIO_WritePin(DRV_INHB_GPIO_Port, DRV_INHB_Pin, state == FetState::CLOSE ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Drv8323rs::set_phase_b_low(FetState state) {
	HAL_GPIO_WritePin(DRV_INLB_GPIO_Port, DRV_INLB_Pin, state == FetState::CLOSE ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Drv8323rs::set_phase_c_high(FetState state) {
	HAL_GPIO_WritePin(DRV_INHC_GPIO_Port, DRV_INHC_Pin, state == FetState::CLOSE ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Drv8323rs::set_phase_c_low(FetState state) {
	HAL_GPIO_WritePin(DRV_INLC_GPIO_Port, DRV_INLC_Pin, state == FetState::CLOSE ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool Drv8323rs::has_fault() {
	return HAL_GPIO_ReadPin(DRV_nFAULT_GPIO_Port, DRV_nFAULT_Pin);
}

Drv8323rs::DriverFaults Drv8323rs::get_faults() {
	uint16_t faults1 = read_spi(ADDR_FAULT_REGISTER_1);
	uint16_t faults2 = read_spi(ADDR_FAULT_REGISTER_2);
	return DriverFaults{faults1, faults2};
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
