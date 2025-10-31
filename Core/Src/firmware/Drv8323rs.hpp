/*
 * Drv8323rs.hpp
 *
 *  Created on: Oct 30, 2025
 *      Author: jakob
 */

#pragma once
#include "main.h"

namespace Drv8323rs {
enum PwmControlMode {
	X6 = 0,
	X3 = 1,
	X1 = 2,
	INDEPENDENT = 3
};
struct DriverControl {


};
struct DriverFaults {
	bool fault = false;
	bool overcurrent = false;
	bool gate_driver = false;
	bool undervoltage_lockout = false;
	bool overtemp_shutdown = false;
	bool overcurrent_a_high = false;
	bool overcurrent_a_low = false;
	bool overcurrent_b_high = false;
	bool overcurrent_b_low = false;
	bool overcurrent_c_high = false;
	bool overcurrent_c_low = false;
	bool overcurrent_sense_a = false;
	bool overcurrent_sense_b = false;
	bool overcurrent_sense_c = false;
	bool overtemp_warning = false;
	bool chargepump_undervoltage = false;
	bool drive_a_high = false;
	bool drive_a_low = false;
	bool drive_b_high = false;
	bool drive_b_low = false;
	bool drive_c_high = false;
	bool drive_c_low = false;


	DriverFaults(uint16_t bits1, uint16_t bits2) : fault((bits1 >> 10) & 1), overcurrent((bits1 >> 9) & 1),
			gate_driver((bits1 >> 8) & 1), undervoltage_lockout((bits1 >> 7) & 1),
			overtemp_shutdown((bits1 >> 6) & 1),
			overcurrent_a_high((bits1 >> 5) & 1), overcurrent_a_low((bits1 >> 4) & 1),
			overcurrent_b_high((bits1 >> 3) & 1), overcurrent_b_low((bits1 >> 2) & 1),
			overcurrent_c_high((bits1 >> 1) & 1), overcurrent_c_low((bits1 >> 0) & 1),
			overcurrent_sense_a((bits2 >> 10) & 1), overcurrent_sense_b((bits2 >> 9) & 1),
			overcurrent_sense_c((bits2 >> 8) & 1),
			overtemp_warning((bits2 >> 7) & 1), chargepump_undervoltage((bits2 >> 6) & 1),
			drive_a_high((bits2 >> 5) & 1), drive_a_low((bits2 >> 4) & 1),
			drive_b_high((bits2 >> 3) & 1), drive_b_low((bits2 >> 2) & 1),
			drive_c_high((bits2 >> 1) & 1), drive_c_low((bits2 >> 0) & 1){}
};
enum FetState {
	OPEN,
	CLOSE
};
void init(SPI_HandleTypeDef &spi);
void read_write_spi(uint16_t &rx, uint16_t &tx);
uint16_t read_spi(uint16_t addr);
PwmControlMode set_pwm_mode(PwmControlMode new_mode);
void set_phase_a_high(FetState state);
void set_phase_a_low(FetState state);
void set_phase_b_high(FetState state);
void set_phase_b_low(FetState state);
void set_phase_c_high(FetState state);
void set_phase_c_low(FetState state);
void sleep(const bool enable);
void reset_fault_hw();
bool has_fault();
DriverFaults get_faults();
};

