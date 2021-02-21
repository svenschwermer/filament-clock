## Notes

## Problems

### 2nd digit is blank
- The RTC sometimes has garbage in its registers after power-up, e.g. a value >
  9 in one of the BCD nibbles.
- Handling all integers in the TLS59108 driver makes this clear.

### Separator doesn't blink
- Separator MCU didn't ACK its configuration words.
- Activating 4.7 kOhm pull-ups on the I2CDriver made the problem go away =>
  lower pull-up resistance is needed on the I2C lines. 4.7 kOhm is not
  sufficient.
