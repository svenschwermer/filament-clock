#ifndef _TLC59108_H_
#define _TLC59108_H_

#include <stdint.h>

#define TLC59108_ADDR(x) ((0x40+(x)) << 1) // write address
/*
0x48 - default all-call address
0x49..0x4c - default sub-call addresses (default disabled)
0x4b - software reset address
*/

void tlc59108_init(uint8_t addr);
void tlc59108_digit(uint8_t addr, char dig);

#endif
