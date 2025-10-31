/*
 * Stm32f05.h
 *
 *  Created on: Oct 31, 2025
 *      Author: jakob
 */

#pragma once
#include "main.h"

namespace Stm32f05 {
void init();
void set_led_1(const bool enable);
void set_led_2(const bool enable);
void set_led_3(const bool enable);

} /* namespace Stm32f05 */

