#ifndef _BCD_H_
#define _BCD_H_

#include <stdbool.h>
#include <stdint.h>

bool incr_bcd(uint8_t *x, uint8_t max);
void incr_time(uint8_t *s, uint8_t *m);

#endif
