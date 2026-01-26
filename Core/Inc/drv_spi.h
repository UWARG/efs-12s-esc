#ifndef DRV_SPI_H
#define DRV_SPI_H

#include <stdint.h>
#include <stdbool.h>

/* ========= API ========= */

/* Initialize SPI + driver control GPIOs */
void drv_spi_init(void);

/* Write gate driver register */
bool drv_spi_write(uint8_t reg, uint16_t value);

/* Read gate driver register */
bool drv_spi_read(uint8_t reg, uint16_t *value);

/* Read and return raw fault register */
uint16_t drv_spi_read_fault(void);

/* Clear latched faults (if supported by driver) */
void drv_spi_clear_fault(void);

/* Enable / disable gate driver */
void drv_enable(bool enable);

/* Perform one-time driver configuration */
bool drv_configure(void);

#endif /* DRV_SPI_H */
