#include "tlc59108.h"
#include "twi.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#define TLC59108_ADDR(x) ((0x40+x) << 1) // write address
#define PCF8523_ADDR (0x68 << 1) // write address
#define SEG0_ADDR (0x13 << 1)

bool incr_bcd(uint8_t *x, uint8_t max)
{
    uint8_t i = 10 * (*x >> 4) + (*x & 0x0f);
    if (++i > max)
    {
        *x = 0;
        return true;
    }
    *x = ((i / 10) << 4) | (i % 10);
    return false;
}

void incr_time(uint8_t *s, uint8_t *m)
{
    if (incr_bcd(s, 59))
        incr_bcd(m, 59);
}

int main(void)
{
    // No clock division
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);

    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    twi_init();

    for (uint8_t i=0; i<4; ++i)
        tlc59108_init(TLC59108_ADDR(i));

    uint8_t seg_init_data[] = {0xff, 0x00, 0x40};
    twi_regwr(SEG0_ADDR, seg_init_data, sizeof(seg_init_data));

    struct { uint8_t seconds, minutes; } r = {0, 0};
    while (1)
    {
#if 1
        twi_regrd(PCF8523_ADDR, 0x03, &r, sizeof r);
        //incr_time(&r.seconds, &r.minutes);
        _delay_ms(100); // TODO: fix
        tlc59108_digit(TLC59108_ADDR(0), (r.minutes & 0x70) >> 4);
        tlc59108_digit(TLC59108_ADDR(1), r.minutes & 0x0f);
        tlc59108_digit(TLC59108_ADDR(2), (r.seconds & 0x70) >> 4);
        tlc59108_digit(TLC59108_ADDR(3), r.seconds & 0x0f);
#else
        for (uint8_t dig=0; dig<10; ++dig)
        {
            for (int i=0; i<4; ++i)
                tlc59108_digit(TLC59108_ADDR(i), dig);
            _delay_ms(1000);
        }
#endif
    }
}
