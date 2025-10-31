/*
 * Stm32f05.cpp
 *
 *  Created on: Oct 31, 2025
 *      Author: jakob
 */

#include "Stm32f05.hpp"

namespace Stm32f05 {
void init() {}
void set_led_1(const bool enable) {
	HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);

}
void set_led_2(const bool enable) {
	HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
void set_led_3(const bool enable) {
	HAL_GPIO_WritePin(LED_3_GPIO_Port, LED_3_Pin, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


} /* namespace Stm32f05 */
