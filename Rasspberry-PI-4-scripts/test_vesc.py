import serial
import time

# Bytt ut 'COM9' med porten til din USB‑TTL
ser = serial.Serial('COM9', 115200, timeout=1)

# Vent litt om nødvendig
time.sleep(0.1)

# Bygg frame
frame = bytes([0x02, 0x05, 0x05, 0x00, 0x00, 0x00, 0x3F, 0xE4, 0xEB, 0x03])

print("Sender:", frame.hex(' '))
ser.write(frame)

# (Valgfritt) les tilbake om VESC svarer
resp = ser.read(32)
print("Mottatt :", resp.hex(' '))

ser.close()