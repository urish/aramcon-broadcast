#ifndef _VS1053_H
#define _VS1053_H

#include <nrf.h>

#define VS1053_PIN_CS NRF_GPIO_PIN_MAP(0, 8) /* D10 */
#define VS1053_PIN_MISO NRF_GPIO_PIN_MAP(1, 14) /* D11 */ 
#define VS1053_PIN_MOSI NRF_GPIO_PIN_MAP(1, 13) /* D12 */
#define VS1053_PIN_SCK NRF_GPIO_PIN_MAP(1, 15) /* D13 */

#define VS1053_PIN_xReset NRF_GPIO_PIN_MAP(0, 4) /* A1 */
#define VS1053_PIN_DREQ NRF_GPIO_PIN_MAP(0, 28) /* A2 */
#define VS1053_PIN_xDCS NRF_GPIO_PIN_MAP(0, 29) /* A3 */
#define VS1053_PIN_xCS NRF_GPIO_PIN_MAP(0, 30) /* A4 */
#define VS1053_PIN_LED NRF_GPIO_PIN_MAP(1, 12) /* D7 */

uint32_t vs1053_init();
void vs1053_soft_reset();
void vs1053_reset();
void vs1053_test(uint8_t n, uint16_t ms);
void vs1053_set_volume(uint8_t left, uint8_t right);
void vs1053_send_data(uint8_t *buf, uint8_t len);
bool vs1053_is_ready();

#endif /* _VS1053_H */