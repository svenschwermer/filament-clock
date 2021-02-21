#include "twi.h"
#include <avr/io.h>

void twi_init(void)
{
    TWI0.MBAUD = F_CPU / 2 / F_SCL - F_CPU / 2 * T_SCL_RISE - 5;
    TWI0.MCTRLA = TWI_SMEN_bm | TWI_ENABLE_bm;
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

void twi_regwr(uint8_t addr, const uint8_t *reg_data, uint8_t len)
{
    TWI0.MADDR = addr;
    while ((TWI0.MSTATUS & TWI_WIF_bm) == 0x00);
    
    while (len-- > 0)
    {
        TWI0.MDATA = *reg_data++;
        while ((TWI0.MSTATUS & TWI_WIF_bm) == 0x00);
    }

    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

void twi_regwr_single(uint8_t addr, uint8_t reg, uint8_t val)
{
    uint8_t reg_data[] = {reg, val};
    twi_regwr(addr, reg_data, 2);
}

void twi_regrd(uint8_t addr, uint8_t reg, void *const data, uint8_t len)
{
    uint8_t *buf = (uint8_t*)data;
    twi_regwr(addr, &reg, 1);

    TWI0.MADDR = addr | 0x01;
    
    while (len-- > 0)
    {
        while ((TWI0.MSTATUS & TWI_RIF_bm) == 0x00);
        if (len == 0)
            TWI0.MCTRLB |= TWI_ACKACT_bm;
        else
            TWI0.MCTRLB &= ~TWI_ACKACT_bm;
        *buf++ = TWI0.MDATA;
    }

    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}
