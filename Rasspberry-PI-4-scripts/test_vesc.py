import serial
import time
import struct

def vesc_crc(data):
    crc = 0
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ 0x1021
            else:
                crc <<= 1
            crc &= 0xFFFF
    return crc

def make_packet(payload):
    payload_len = len(payload)
    if payload_len <= 256:
        packet = bytearray()
        packet.append(2)  # Start byte
        packet.append(payload_len)
        packet.extend(payload)
        crc = vesc_crc(payload)
        packet.append((crc >> 8) & 0xFF)
        packet.append(crc & 0xFF)
        packet.append(3)  # End byte
        return packet
    else:
        raise ValueError("Payload too long")

def set_erpm(erpm):
    COMM_SET_RPM = 5
    payload = struct.pack(">Bi", COMM_SET_RPM, int(erpm))
    return make_packet(payload)

# === KONFIG ===
DEVICE = "/dev/ttyAMA0"  # Bruk riktig device for UART
BAUD = 115200

# === KJØRING ===
ser = serial.Serial(DEVICE, BAUD, timeout=1)

print("Kjører motor på 9000 ERPM i 5 sekunder")
start = time.time()

try:
    while time.time() - start < 5:
        ser.write(set_erpm(9000))
        time.sleep(0.05)  # 20 Hz
finally:
    print("Stopper motor")
    ser.write(set_erpm(0))
    ser.close()
