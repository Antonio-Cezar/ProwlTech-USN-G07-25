#!/usr/bin/env python3
import serial
import struct
import time

SERIAL_PORT = '/dev/ttyS0'     # TTL-UART på GPIO-pins (GPIO14/15)
BAUDRATE    = 9600
TIMEOUT     = 1                # sekunder

def parse_frame(frame: bytes):
    """
    Tar en 16-byte frame som:
      [0]    = 0xA5 (header)
      [1]    = SOC (%)
      [2-3]  = Voltage * 100 (big-endian)
      [4-7]  = Capacity mAh (uint32 big-endian)
      [8-11] = Current mA (int32 big-endian)
      [12-14]= Remaining time sekunder (24-bit)
      [15]   = checksum (sum av byte 0–14 mod 256)
    Returnerer dict eller None ved feil.
    """
    if len(frame) != 16 or frame[0] != 0xA5:
        return None

    # Sjekk checksum
    if (sum(frame[0:15]) & 0xFF) != frame[15]:
        return None

    soc = frame[1]
    voltage_raw = (frame[2] << 8) | frame[3]
    voltage = voltage_raw / 100.0  # Volt

    capacity = struct.unpack('>I', frame[4:8])[0]  # mAh
    current  = struct.unpack('>i', frame[8:12])[0] # mA

    t = (frame[12] << 16) | (frame[13] << 8) | frame[14]
    hrs, rem = divmod(t, 3600)
    mins, secs = divmod(rem, 60)
    remaining = f"{hrs:02d}:{mins:02d}:{secs:02d}"

    return {
        'soc'       : soc,
        'voltage'   : voltage,
        'capacity'  : capacity,
        'current'   : current,
        'remaining' : remaining
    }

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=TIMEOUT)
        print(f"Åpnet {SERIAL_PORT} @ {BAUDRATE} bps")
    except serial.SerialException as e:
        print("Kunne ikke åpne seriel port:", e)
        return

    buf = bytearray()
    try:
        while True:
            b = ser.read(1)
            if not b:
                continue

            # Synkroniser på header-byte 0xA5
            if not buf and b[0] != 0xA5:
                continue

            buf += b

            if len(buf) == 16:
                data = parse_frame(buf)
                if data:
                    print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] "
                          f"SOC = {data['soc']}%  |  "
                          f"V = {data['voltage']:.2f}V  |  "
                          f"I = {data['current']}mA  |  "
                          f"Time = {data['remaining']}")
                else:
                    print("Ugyldig frame eller checksum-feil")

                # Nullstill buffer og let etter ny header
                buf.clear()

    except KeyboardInterrupt:
        print("\nAvslutter...")
    finally:
        ser.close()

if __name__ == '__main__':
    main()
