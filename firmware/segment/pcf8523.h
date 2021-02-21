#ifndef _PCF8523_H_
#define _PCF8523_H_

#include <stdint.h>

#define PCF8523_ADDR (0x68 << 1) // write address

struct pcf8523_time_regs
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
};

void pcf8523_enable_second_int(void);
void pcf8523_clear_second_int(void);
void pcf8523_get_time(struct pcf8523_time_regs *r);

#endif
