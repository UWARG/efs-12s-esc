/*
 * Drv8323rs.hpp
 *
 *  Created on: Oct 30, 2025
 *      Author: jakob
 */

#pragma once
#include "main.h"

namespace Drv8323rs {
void init();
void sleep(const bool enable);
void reset_fault_hw();
bool read_fault();
};

