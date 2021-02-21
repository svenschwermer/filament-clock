#ifndef _TLC59108_H_
#define _TLC59108_H_

#include <stdint.h>

void tlc59108_init(uint8_t addr);
void tlc59108_digit(uint8_t addr, char dig);

#endif
