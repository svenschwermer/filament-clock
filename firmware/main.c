#include <stddef.h>
#include <avr/io.h>
#include <util/delay.h>

#define F_SCL 100000ul // 100kHz
#define T_SCL_RISE 44ul / 100000000ul // 440ns

#define TLC59108_ADDR (0x40 << 1) // write address
#define PCF8523_ADDR (0x68 << 1) // write address

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

void twi_regrd(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
    twi_regwr(addr, &reg, 1);

    TWI0.MADDR = addr | 0x01;
    
    while (len-- > 0)
    {
        while ((TWI0.MSTATUS & TWI_RIF_bm) == 0x00);
        if (len == 0)
            TWI0.MCTRLB |= TWI_ACKACT_bm;
        else
            TWI0.MCTRLB &= ~TWI_ACKACT_bm;
        *data++ = TWI0.MDATA;
    }

    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

void tlc59108_init(void)
{
    // MODE1: Enable Register Auto-Increment, Normal Mode
    twi_regwr_single(TLC59108_ADDR, 0x00, 0x81);
    // MODE2: DMBLNK=0 => Group control = dimming
    twi_regwr_single(TLC59108_ADDR, 0x01, 0x81);
    // GRPPWM=1
    twi_regwr_single(TLC59108_ADDR, 0x0A, 0x01);
    // PWMx=255 (maxed out)
    for (uint8_t reg = 0x02; reg <= 0x09; ++reg)
        twi_regwr_single(TLC59108_ADDR, reg, 0xFF);
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
    case '-':
        data[2] = 0x03;
        break;
    }

    twi_regwr(addr, data, 3);
}

int main(void)
{
    // No clock division
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);

    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    twi_init();
    tlc59108_init();

    while (1)
    {
        uint8_t seconds;
        twi_regrd(PCF8523_ADDR, 0x03, &seconds, 1);
        tlc59108_digit(TLC59108_ADDR, seconds & 0x0F);
    }
}
