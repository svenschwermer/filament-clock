import i2cdriver
from time import sleep

# segment (Dx) -> register offset
segs = {
    1: 0x2,
    2: 0x3,
    3: 0x4,
    4: 0x7,
    5: 0x8,
    6: 0x9,
    7: 0x6,
}
chars = {
    0: [1, 2, 3, 4, 5, 6],
    1: [2, 3],
    2: [1, 2, 4, 5, 7],
    3: [1, 2, 3, 4, 7],
    4: [2, 3, 6, 7],
    5: [1, 3, 4, 6, 7],
    6: [1, 3, 4, 5, 6, 7],
    7: [1, 2, 3],
    8: [1, 2, 3, 4, 5, 6, 7],
    9: [1, 2, 3, 4, 6, 7],
    0xA: [1, 2, 3, 5, 6, 7],
    0xB: [3, 4, 5, 6, 7],
    0xC: [1, 4, 5, 6],
    0xD: [2, 3, 4, 5, 7],
    0xE: [1, 4, 5, 6, 7],
    0xF: [1, 5, 6, 7],
}

def char(c):
    set_segs = []
    for s in chars[c]:
        set_segs.append(s)
        i2c.regwr(0x40, segs[s], 8)
    for s in range(1, 8):
        if s not in set_segs:
            i2c.regwr(0x40, segs[s], 0)

def foo(c):
    v = [0] * 8
    for s in chars[c]:
        v[segs[s]-2] = 8
    i2c.regwr(0x40, 0x82, v)


i2c = i2cdriver.I2CDriver()

# enable oscillator
i2c.regwr(0x40, 0x0, 0x01)

# enable all outputs
i2c.regwr(0x40, 0xc, 0xff)
i2c.regwr(0x40, 0xd, 0xff)

while True:
    for i in range(16):
        foo(i)
        sleep(0.25)
