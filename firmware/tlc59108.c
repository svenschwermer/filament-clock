#include "tlc59108.h"
#include "twi.h"

void tlc59108_init(uint8_t addr)
{
    // MODE1: Enable Register Auto-Increment, Normal Mode
    twi_regwr_single(addr, 0x00, 0x81);
    // MODE2: DMBLNK=0 => Group control = dimming
    twi_regwr_single(addr, 0x01, 0x81);
    // GRPPWM=1
    twi_regwr_single(addr, 0x0A, 0x01);
    // PWMx=255 (maxed out)
    for (uint8_t reg = 0x02; reg <= 0x09; ++reg)
        twi_regwr_single(addr, reg, 0xFF);
}

void tlc59108_digit(uint8_t addr, char dig)
{
    /*
    Segment 1: LED0
    Segment 2: LED1
    Segment 3: LED2
    Segment 4: LED5
    Segment 5: LED6
    Segment 6: LED7
    Segment 7: LED4

    LEDOUT0: ---- Seg3 Seg2 Seg1
    LEDOUT1: Seg6 Seg5 Seg4 Seg7
    */

    uint8_t data[] = {0x8C, 0x00, 0x00};
    switch (dig)
    {
    case 0:
        // Segments: 1 2 3 4 5 6
        data[1] = 0x3F;
        data[2] = 0xFC;
        break;
    case 1:
        // Segments: 2 3
        data[1] = 0x3C;
        break;
    case 2:
        // Segments: 1 2 4 5 7
        data[1] = 0x0F;
        data[2] = 0x3F;
        break;
    case 3:
        // Segments: 1 2 3 4 7
        data[1] = 0x3F;
        data[2] = 0x0F;
        break;
    case 4:
        // Segments: 2 3 6 7
        data[1] = 0x3C;
        data[2] = 0xC3;
        break;
    case 5:
        // Segments: 1 3 4 6 7
        data[1] = 0x33;
        data[2] = 0xCF;
        break;
    case 6:
        // Segments: 1 3 4 5 6 7
        data[1] = 0x33;
        data[2] = 0xFF;
        break;
    case 7:
        // Segments: 1 2 3
        data[1] = 0x3F;
        break;
    case 8:
        // Segments: 1 2 3 4 5 6 7
        data[1] = 0x3F;
        data[2] = 0xFF;
        break;
    case 9:
        // Segments: 1 2 3 4 6 7
        data[1] = 0x3F;
        data[2] = 0xCF;
        break;
    default:
        // Segment: 7 (dash)
        data[2] = 0x03;
        break;
    }

    twi_regwr(addr, data, 3);
}
