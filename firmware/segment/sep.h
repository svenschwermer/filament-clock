#ifndef _SEP_H_
#define _SEP_H_

#include <stdint.h>

#define SEP_ADDR(x) ((0x10+(x)) << 1)

void sep_config(uint8_t addr, uint16_t brightness, uint8_t blink_addr);

#endif
