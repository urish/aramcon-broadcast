#ifndef _VS1053_H
#define _VS1053_H

#include <nrf.h>

#define VS1053_PIN_MISO NRF_GPIO_PIN_MAP(1, 9)
#define VS1053_PIN_MOSI NRF_GPIO_PIN_MAP(1, 10)
#define VS1053_PIN_SCK NRF_GPIO_PIN_MAP(0, 1)

#define VS1053_PIN_xReset NRF_GPIO_PIN_MAP(0, 0)
#define VS1053_PIN_DREQ NRF_GPIO_PIN_MAP(0, 13)
#define VS1053_PIN_xDCS NRF_GPIO_PIN_MAP(0, 24)
#define VS1053_PIN_xCS NRF_GPIO_PIN_MAP(1, 13)
#define VS1053_PIN_LED NRF_GPIO_PIN_MAP(1, 11)

uint32_t vs1053_init();
void vs1053_soft_reset();
void vs1053_reset();
void vs1053_test(uint8_t n, uint16_t ms);
void vs1053_set_volume(uint8_t left, uint8_t right);
void vs1053_send_data(uint8_t *buf, uint8_t len);
bool vs1053_is_ready();

#endif /* _VS1053_H */