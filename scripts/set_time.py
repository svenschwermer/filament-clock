from argparse import ArgumentParser
from datetime import datetime
from i2cdriver import I2CDriver


class PCF8523:
    class WriteError(Exception):
        pass

    def __init__(self, address=0x68, port='/dev/ttyUSB0'):
        self.address = address
        self.i2c = I2CDriver(port=port)

    @staticmethod
    def _to_bcd(i: int) -> int:
        return (int(i/10) << 4) | (i % 10)

    def regwr(self, reg, data):
        if not self.i2c.regwr(self.address, reg, data):
            raise self.WriteError

    def reset(self):
        self.regwr(0x00, 0x58)

    def set_time(self, dt):
        self.regwr(0x03, [
            self._to_bcd(dt.second),
            self._to_bcd(dt.minute),
            self._to_bcd(dt.hour),
            self._to_bcd(dt.day),
            self._to_bcd((dt.weekday()+1) % 7),
            self._to_bcd(dt.month),
            self._to_bcd(dt.year % 100),
        ])


if __name__ == '__main__':
    parser = ArgumentParser(description='Set RTC to the current time')
    parser.add_argument('--reset', action='store_true',
                        help='Reset RTC before setting the time')
    args = parser.parse_args()

    rtc = PCF8523()
    if args.reset:
        rtc.reset()
    rtc.set_time(datetime.now())
