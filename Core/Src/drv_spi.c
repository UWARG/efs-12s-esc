#include "drv_spi.h"
#include "stm32f0xx_hal.h"
#include "stm32f051x8.h"

/* ========= External handles ========= */
extern SPI_HandleTypeDef hspi1;

/* ========= GPIO mapping ========= */
/* From your schematic */
#define DRV_EN_GPIO_Port     GPIOC
#define DRV_EN_Pin           GPIO_PIN_6

#define DRV_CAL_GPIO_Port    GPIOC
#define DRV_CAL_Pin          GPIO_PIN_7

#define DRV_NFAULT_GPIO_Port GPIOC
#define DRV_NFAULT_Pin       GPIO_PIN_8

#define SPI_NSS_GPIO_Port    GPIOA
#define SPI_NSS_Pin          GPIO_PIN_15

/* ========= Protocol ========= */
/*
 * 16-bit SPI frame:
 * [15]    R/W (1 = read, 0 = write)
 * [14:11] Register address
 * [10:0]  Data
 *
 * Adjust if your driver differs.
 */
#define DRV_SPI_READ_MASK    0x8000
#define DRV_SPI_ADDR_SHIFT   11
#define DRV_SPI_DATA_MASK    0x07FF

/* ========= Helpers ========= */

static inline void nss_low(void)
{
    HAL_GPIO_WritePin(SPI_NSS_GPIO_Port, SPI_NSS_Pin, GPIO_PIN_RESET);
}

static inline void nss_high(void)
{
    HAL_GPIO_WritePin(SPI_NSS_GPIO_Port, SPI_NSS_Pin, GPIO_PIN_SET);
}

static bool spi_transfer(uint16_t tx, uint16_t *rx)
{
    uint8_t tx_buf[2] = { tx >> 8, tx & 0xFF };
    uint8_t rx_buf[2] = { 0 };

    nss_low();

    if (HAL_SPI_TransmitReceive(&hspi1, tx_buf, rx_buf, 2, 10) != HAL_OK) {
        nss_high();
        return false;
    }

    nss_high();

    if (rx)
        *rx = ((uint16_t)rx_buf[0] << 8) | rx_buf[1];

    return true;
}

/* ========= Public API ========= */

void drv_spi_init(void)
{
    /* NSS idle high */
    nss_high();

    /* Disable driver initially */
    drv_enable(false);
}

bool drv_spi_write(uint8_t reg, uint16_t value)
{
    uint16_t frame =
        ((reg & 0x0F) << DRV_SPI_ADDR_SHIFT) |
        (value & DRV_SPI_DATA_MASK);

    return spi_transfer(frame, NULL);
}

bool drv_spi_read(uint8_t reg, uint16_t *value)
{
    uint16_t frame =
        DRV_SPI_READ_MASK |
        ((reg & 0x0F) << DRV_SPI_ADDR_SHIFT);

    uint16_t rx;
    if (!spi_transfer(frame, &rx))
        return false;

    if (value)
        *value = rx & DRV_SPI_DATA_MASK;

    return true;
}

uint16_t drv_spi_read_fault(void)
{
    uint16_t fault = 0;
    drv_spi_read(0x01, &fault);  /* Example FAULT register */
    return fault;
}

void drv_spi_clear_fault(void)
{
    /* Many drivers clear faults by writing 0 or toggling EN */
    drv_enable(false);
    HAL_Delay(1);
    drv_enable(true);
}

void drv_enable(bool enable)
{
    HAL_GPIO_WritePin(DRV_EN_GPIO_Port,
                      DRV_EN_Pin,
                      enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool drv_configure(void)
{
    /* Example configuration sequence
     * (values MUST match your driver datasheet)
     */

    drv_enable(false);
    HAL_Delay(1);

    /* Calibration mode */
    HAL_GPIO_WritePin(DRV_CAL_GPIO_Port, DRV_CAL_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(DRV_CAL_GPIO_Port, DRV_CAL_Pin, GPIO_PIN_RESET);

    /* Deadtime, OCP, gate strength, etc */
    if (!drv_spi_write(0x02, 0x0123)) return false;
    if (!drv_spi_write(0x03, 0x0045)) return false;
    if (!drv_spi_write(0x04, 0x000A)) return false;

    drv_enable(true);
    HAL_Delay(1);

    /* Check fault pin */
    if (HAL_GPIO_ReadPin(DRV_NFAULT_GPIO_Port, DRV_NFAULT_Pin) == GPIO_PIN_RESET)
        return false;

    return true;
}
