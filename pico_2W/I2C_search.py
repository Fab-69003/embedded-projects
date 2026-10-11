from machine import I2C, Pin

i2c = I2C(0, sda=Pin(4), scl=Pin(5), freq=100000)
print([hex(a) for a in i2c.scan()])