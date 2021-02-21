#include "pcf8523.h"
#include "twi.h"

void pcf8523_enable_second_int(void)
{
    // Control_1: enable second interrupt
    twi_regwr_single(PCF8523_ADDR, 0x00, 0x04);

    // Tmr_CLKOUT_ctrl: disable CLKOUT
    twi_regwr_single(PCF8523_ADDR, 0x0f, 0x38);
}

void pcf8523_clear_second_int(void)
{
    // Control_2: reset second interrupt flag
    twi_regwr_single(PCF8523_ADDR, 0x01, 0x00);
}

void pcf8523_get_time(struct pcf8523_time_regs *r)
{
    twi_regrd(PCF8523_ADDR, 0x03, r, sizeof *r);
}
