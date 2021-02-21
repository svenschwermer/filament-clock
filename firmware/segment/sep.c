#include "sep.h"
#include "twi.h"

void sep_config(uint8_t addr, uint16_t brightness, uint8_t blink_addr)
{
    uint8_t init_data[] = {brightness & 0xff, brightness >> 8, blink_addr};
    twi_regwr(addr, init_data, sizeof(init_data));
}
