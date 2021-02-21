#include "pcf8523.h"
#include "sep.h"
#include "tlc59108.h"
#include "twi.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#define TLC59108_COUNT 4

int main(void)
{
    // No clock division
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);

    // Button pull-ups
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    twi_init();

    for (uint8_t i=0; i<TLC59108_COUNT; ++i)
        tlc59108_init(TLC59108_ADDR(i));

    sep_config(SEP0_ADDR, 0x00ff, TLC59108_ADDR(0) >> 1);

    while (1)
    {
#if 1
        struct { uint8_t seconds, minutes; } r = {0, 0};
        twi_regrd(PCF8523_ADDR, 0x03, &r, sizeof r);
        tlc59108_digit(TLC59108_ADDR(0), (r.minutes & 0x70) >> 4);
        tlc59108_digit(TLC59108_ADDR(1), r.minutes & 0x0f);
        tlc59108_digit(TLC59108_ADDR(2), (r.seconds & 0x70) >> 4);
        tlc59108_digit(TLC59108_ADDR(3), r.seconds & 0x0f);
        _delay_ms(1000); // TODO: fix
#else
        for (uint8_t dig=0; dig<10; ++dig)
        {
            for (uint8_t i=0; i<TLC59108_COUNT; ++i)
                tlc59108_digit(TLC59108_ADDR(i), dig);
            _delay_ms(1000);
        }
#endif
    }
}
