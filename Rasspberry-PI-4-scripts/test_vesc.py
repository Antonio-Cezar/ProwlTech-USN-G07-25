import serial
import time

# Bruk riktig enhet her:
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

# Gi litt tid til at UARTen kommer opp
time.sleep(0.1)

# Bygg VESC‑rute
frame = bytes([0x02, 0x05, 0x05, 0x00, 0x00, 0x00, 0x3F, 0xE4, 0xEB, 0x03])

print("Sender:", frame.hex(' '))
ser.write(frame)

# (Valgfritt) les eventuelt svar fra VESC
resp = ser.read(32)
print("Mottatt:", resp.hex(' '))

ser.close()
