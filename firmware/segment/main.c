#include "pcf8523.h"
#include "sep.h"
#include "tlc59108.h"
#include "twi.h"
#include <avr/io.h>

#define TLC59108_COUNT 4

int main(void)
{
    // No clock division
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);

    // Button/RTC pull-ups
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    twi_init();

    for (uint8_t i=0; i<TLC59108_COUNT; ++i)
        tlc59108_init(TLC59108_ADDR(i));

    sep_config(SEP_ADDR(0), 0x00ff, TLC59108_ADDR(0) >> 1);

    pcf8523_enable_second_int();

    while (1)
    {
        // wait for RTC interrupt...
        while(PORTA.IN & PIN6_bm);

        struct pcf8523_time_regs r;
        pcf8523_get_time(&r);

#if TLC59108_COUNT == 6
        tlc59108_digit(TLC59108_ADDR(0), (r.hours & 0x30) >> 4);
        tlc59108_digit(TLC59108_ADDR(1), r.hours & 0x0f);
#endif
        tlc59108_digit(TLC59108_ADDR(0), (r.minutes & 0x70) >> 4);
        tlc59108_digit(TLC59108_ADDR(1), r.minutes & 0x0f);
        tlc59108_digit(TLC59108_ADDR(2), (r.seconds & 0x70) >> 4);
        tlc59108_digit(TLC59108_ADDR(3), r.seconds & 0x0f);

        pcf8523_clear_second_int();
    }
}
