#ifndef _TWI_H_
#define _TWI_H_

#include <stdint.h>

#define F_SCL 100000ul // 100kHz
#define T_SCL_RISE 44ul / 100000000ul // 440ns

void twi_init(void);
void twi_regwr(uint8_t addr, const uint8_t *reg_data, uint8_t len);
void twi_regwr_single(uint8_t addr, uint8_t reg, uint8_t val);
void twi_regrd(uint8_t addr, uint8_t reg, void *const data, uint8_t len);

#endif
