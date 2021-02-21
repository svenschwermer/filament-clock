#include "bcd.h"

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
