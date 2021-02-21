#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

/* REGISTER MAP

The register contents are written immediately after the address byte. That means
that writting register 0x03 requires also writing registers 0x00 and 0x01.

[0x00] - Brightness LSB
[0x01] - Brightness MSB
[0x02] - Autoblink address
         To deactivate, set to 0x00. If enabled, the segment will automatically
         blink when this address is written to. This device will not ackowledge
         the address byte. Since this is a 7-bit address, the top-most bit is
         ignored.
*/

#define ADDRESS_BASE 0x10 // 7-bit address: 0x10..0x13

static enum {
    fsm_brightness_lsb,
    fsm_brightness_msb,
    fsm_autoblink_addr,
} fsm;

int main(void)
{
    // No clock division
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, !CLKCTRL_PEN_bm);

    PORTA.DIRSET = PIN3_bm;
    // PWN frequency: 20,000,000 Hz / (2^16 + 1) / 1 = ~305 Hz
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
    TCA0.SINGLE.CMP0 = 1024;

    // Waiting for both status registers to clear is crucial. A lot of head-
    // scratching went into the solution of this mystery.
    while (RTC.STATUS != 0 || RTC.PITSTATUS != 0);
    RTC.CLKSEL = 1; // 1024 Hz
    RTC.PER = 512; // 500 ms
    RTC.INTCTRL = RTC_OVF_bm;
    sei();

    // Enable pull-up resistors in case the A0 / A1 pins are not soldered to
    // either VCC or GND.
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
    TWI0.SADDR = (ADDRESS_BASE + (PORTA.IN >> 6)) << 1;
    TWI0.SCTRLA = TWI_ENABLE_bm;

    while (1)
    {
        if (TWI0.SSTATUS & TWI_APIF_bm)
        {
            // We don't support reading
            if (TWI0.SSTATUS & TWI_DIR_bm)
                TWI0.SCTRLB = TWI_ACKACT_NACK_gc | TWI_SCMD_RESPONSE_gc;
            else if (TWI0.SDATA == TWI0.SADDR)
            {
                // ACK and start write FSM
                TWI0.SCTRLB = TWI_ACKACT_ACK_gc | TWI_SCMD_RESPONSE_gc;
                fsm = fsm_brightness_lsb;
            }
            else
            {
                // blink but NAK
                TWI0.SCTRLB = TWI_ACKACT_NACK_gc | TWI_SCMD_RESPONSE_gc;
                TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm;
                RTC.CTRLA = RTC_RTCEN_bm;
            }
        }

        if (TWI0.SSTATUS & TWI_DIF_bm)
        {
            switch (fsm++)
            {
            case fsm_brightness_lsb:
                TWI0.SCTRLB = TWI_ACKACT_ACK_gc | TWI_SCMD_RESPONSE_gc;
                TCA0.SPLIT.LCMP0 = TWI0.SDATA;
                break;
            case fsm_brightness_msb:
                TWI0.SCTRLB = TWI_ACKACT_ACK_gc | TWI_SCMD_RESPONSE_gc;
                TCA0.SPLIT.HCMP0 = TWI0.SDATA;
                break;
            case fsm_autoblink_addr:
                TWI0.SCTRLB = TWI_ACKACT_ACK_gc | TWI_SCMD_COMPTRANS_gc;
                if (TWI0.SDATA == 0)
                    TWI0.SADDRMASK = 0;
                else
                    TWI0.SADDRMASK = (TWI0.SDATA << 1) | 0x01;
                break;
            }
        }
    }
}

ISR(RTC_CNT_vect)
{
    RTC.INTFLAGS = RTC_OVF_bm; // clear interrupt flag
    RTC.CTRLA = 0; // disable RTC timer
    RTC.CNT = 0; // reset RTC timer count register
    TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP0EN_bm;
}
