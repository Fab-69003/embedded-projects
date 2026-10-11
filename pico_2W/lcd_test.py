from machine import I2C, Pin
import time

class LCD:
    def __init__(self, i2c, addr=0x27):
        self.i2c, self.addr, self.bl = i2c, addr, 0x08
        time.sleep_ms(50)
        for _ in range(3):
            self._write4(0x30, 0)
            time.sleep_ms(5)
        self._write4(0x20, 0)
        self._cmd(0x28)  # 4 bits, 2 lignes
        self._cmd(0x0C)  # écran on, curseur off
        self._cmd(0x06)
        self.clear()

    def _pulse(self, d):
        self.i2c.writeto(self.addr, bytes([d | 0x04 | self.bl]))
        time.sleep_us(2)
        self.i2c.writeto(self.addr, bytes([(d & ~0x04) | self.bl]))
        time.sleep_us(50)

    def _write4(self, nibble, rs):
        self._pulse((nibble & 0xF0) | rs)

    def _send(self, byte, rs):
        self._write4(byte & 0xF0, rs)
        self._write4((byte << 4) & 0xF0, rs)

    def _cmd(self, c):
        self._send(c, 0)

    def clear(self):
        self._cmd(0x01)
        time.sleep_ms(2)

    def move_to(self, col, row):
        self._cmd(0x80 | (col + (0x40 if row else 0)))

    def putstr(self, s):
        for ch in s:
            self._send(ord(ch), 1)

i2c = I2C(0, sda=Pin(4), scl=Pin(5), freq=100000)
lcd = LCD(i2c, 0x27)  # remplace par l'adresse trouvée au scan

lcd.move_to(0, 0)
lcd.putstr("Hello Pico 2 W")
lcd.move_to(0, 1)
lcd.putstr("LCD OK !")