#include <nrf.h>
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_log.h"

#include "vs1053.h"

#define VS_WRITE_COMMAND (0x02)
#define VS_READ_COMMAND (0x03)

#define SPI_MODE    (0x0)
#define SPI_STATUS  (0x1)
#define SPI_BASS    (0x2)
#define SPI_CLOCKF  (0x3)
#define SPI_DECODE_TIME  (0x4)
#define SPI_AUDATA  (0x5)
#define SPI_WRAM    (0x6)
#define SPI_WRAMADDR (0x7)
#define SPI_HDAT0   (0x8)
#define SPI_HDAT1   (0x9)
#define SPI_AIADDR  (0xa)
#define SPI_VOL     (0xb)
#define SPI_AICTRL0 (0xc)
#define SPI_AICTRL1 (0xd)
#define SPI_AICTRL2 (0xe)
#define SPI_AICTRL3 (0xf)

#define SM_DIFF     (0x01)
#define SM_JUMP     (0x02)
#define SM_RESET    (0x04)
#define SM_OUTOFWAV (0x08)
#define SM_PDOWN    (0x10)
#define SM_TESTS    (0x20)
#define SM_STREAM   (0x40)
#define SM_PLUSV    (0x80)
#define SM_DACT     (0x100)
#define SM_SDIORD   (0x200)
#define SM_SDISHARE (0x400)
#define SM_SDINEW   (0x800)
#define SM_ADPCM    (0x1000)
#define SM_ADPCM_HP (0x2000)

#define LOW 0
#define HIGH 1

static const nrf_drv_spi_t m_spi_master_0 = NRF_DRV_SPI_INSTANCE(0);

inline static ret_code_t spi_transfer_and_wait(
                                uint8_t const * p_tx_buffer,
                                uint8_t         tx_buffer_length,
                                uint8_t       * p_rx_buffer,
                                uint8_t         rx_buffer_length) {
    return nrf_drv_spi_transfer(&m_spi_master_0, p_tx_buffer, tx_buffer_length, p_rx_buffer, rx_buffer_length);
}

static uint16_t vs1053_read(uint8_t addr) {
    uint16_t data;
    nrf_gpio_pin_write(VS1053_PIN_xCS, LOW);

    uint8_t buf[4] = { VS_READ_COMMAND, addr, 0, 0 };
    spi_transfer_and_wait(buf, sizeof(buf), buf, sizeof(buf));
    data = (buf[2] << 8) | buf[3];

    nrf_gpio_pin_write(VS1053_PIN_xCS, HIGH);
    return data;
}

static void vs1053_write(uint8_t addr, uint16_t data) {
    nrf_gpio_pin_write(VS1053_PIN_xCS, LOW);
    uint8_t buf[4] = { VS_WRITE_COMMAND, addr, data >> 8, data & 0xFF };
    spi_transfer_and_wait(buf, sizeof(buf), NULL, 0);
    nrf_gpio_pin_write(VS1053_PIN_xCS, HIGH);
}

bool vs1053_is_ready() {
  return nrf_gpio_pin_read(VS1053_PIN_DREQ);
}

static bool vs1053_wait_for_ready() {
    while (!vs1053_is_ready());
}

void vs1053_soft_reset(void) {
    vs1053_write(SPI_MODE, SM_SDINEW | SM_RESET);
    nrf_delay_ms(2);
    vs1053_wait_for_ready();
    vs1053_write(SPI_HDAT0, 0xABAD);
    vs1053_write(SPI_HDAT1, 0x1DEA);
    nrf_delay_ms(100);
    bool sanity_pass = vs1053_read(SPI_HDAT0) == 0xABAD && vs1053_read(SPI_HDAT1) == 0x1DEA;
    printf("VS1053 Sanity: %s\n", sanity_pass ? "PASS" : "FAIL");
    
    vs1053_write(SPI_CLOCKF, 0xC000);
    vs1053_write(SPI_AUDATA, 0xBB81);
    vs1053_write(SPI_BASS, 0x0055);
    vs1053_write(SPI_VOL, 0x4040);
    vs1053_wait_for_ready();
}

void vs1053_set_volume(uint8_t left, uint8_t right) {
    uint16_t v = (left << 8) | right;
    vs1053_write(SPI_VOL, v);
}

void vs1053_reset() {
    nrf_gpio_pin_write(VS1053_PIN_xReset, LOW);
    nrf_delay_ms(100);
    
    nrf_gpio_pin_write(VS1053_PIN_xCS, HIGH);
    nrf_gpio_pin_write(VS1053_PIN_xDCS, HIGH);
    nrf_gpio_pin_write(VS1053_PIN_xReset, HIGH);
    nrf_delay_ms(100);
    vs1053_soft_reset();
    nrf_delay_ms(100);
    
    vs1053_write(SPI_CLOCKF, 0x6000);
    
    vs1053_set_volume(0, 0);
}

void vs1053_test(uint8_t n, uint16_t ms) {
    vs1053_reset();
    uint16_t mode = vs1053_read(SPI_MODE);
    mode |= 0x0020;
    vs1053_write(SPI_MODE, mode);
    
    vs1053_wait_for_ready();
    
    nrf_gpio_pin_write(VS1053_PIN_xDCS, LOW);  
    char buf1[] = { 0x53, 0xEF, 0x6E, n, 0x00, 0x00, 0x00, 0x00 };
    spi_transfer_and_wait(buf1, sizeof(buf1), NULL, 0);
    nrf_gpio_pin_write(VS1053_PIN_xDCS, HIGH);  
    
    nrf_delay_ms(ms);
    
    nrf_gpio_pin_write(VS1053_PIN_xDCS, LOW);  
    char buf2[] = { 0x45, 0x78, 0x69, 0x74, 0x00, 0x00, 0x00, 0x00 };
    spi_transfer_and_wait(buf2, sizeof(buf2), NULL, 0);
    nrf_gpio_pin_write(VS1053_PIN_xDCS, HIGH);  
}

void vs1053_send_data(uint8_t *buf, uint8_t len) {
    nrf_gpio_pin_write(VS1053_PIN_LED, HIGH);
    nrf_gpio_pin_write(VS1053_PIN_xDCS, LOW);
    for (uint16_t i = 0; i < len; i += 32) {
        spi_transfer_and_wait(&buf[i], MIN(32, len - i), NULL, 0);
        vs1053_wait_for_ready();
    }
    nrf_gpio_pin_write(VS1053_PIN_xDCS, HIGH);
    nrf_gpio_pin_write(VS1053_PIN_LED, LOW);
}

uint32_t vs1053_init() {
    uint32_t err_code;
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.frequency = NRF_SPI_FREQ_250K;
    spi_config.miso_pin = VS1053_PIN_MISO;
    spi_config.mosi_pin = VS1053_PIN_MOSI;
    spi_config.sck_pin  = VS1053_PIN_SCK;

    err_code = nrf_drv_spi_init(&m_spi_master_0, &spi_config, NULL, NULL);
    VERIFY_SUCCESS(err_code);

    nrf_gpio_cfg_output(VS1053_PIN_xReset);
    nrf_gpio_cfg_output(VS1053_PIN_xDCS);
    nrf_gpio_cfg_output(VS1053_PIN_xCS);
    nrf_gpio_cfg_output(VS1053_PIN_CS);
    nrf_gpio_cfg_input(VS1053_PIN_DREQ, GPIO_PIN_CNF_PULL_Disabled);
    nrf_gpio_cfg_output(VS1053_PIN_LED);
    nrf_gpio_pin_write(VS1053_PIN_CS, HIGH);
    nrf_gpio_pin_write(VS1053_PIN_xDCS, HIGH);
    vs1053_reset();
}