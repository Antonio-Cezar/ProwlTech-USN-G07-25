#!/usr/bin/env python3
import os
import sys
import time
import struct

import serial

BAUDRATE = 9600
TIMEOUT  = 1  # sekunder

# Liste over vanlige UART-enheter på Raspberry Pi
CANDIDATE_PORTS = [
    '/dev/serial0',
    '/dev/ttyAMA0',
    '/dev/ttyAMA1',
    '/dev/ttyS0',
]

def find_uart_port():
    for port in CANDIDATE_PORTS:
        if os.path.exists(port):
            return port
    return None

def parse_frame(frame: bytes):
    """
    Tar 16-byte ramme med:
      [0]    = 0xA5
      [1]    = SOC (%)
      [2-3]  = spenning*100 (big endian)
      [4-7]  = kapasitet mAh (uint32 BE)
      [8-11] = strøm mA (int32 BE)
      [12-14]= tid igjen i sekunder (24-bit)
      [15]   = checksum (sum 0..14 mod 256)
    Returnerer dict eller None ved feil.
    """
    if len(frame) != 16 or frame[0] != 0xA5:
        return None
    if (sum(frame[0:15]) & 0xFF) != frame[15]:
        return None

    soc = frame[1]
    voltage = ((frame[2] << 8) | frame[3]) / 100.0
    capacity = struct.unpack('>I', frame[4:8])[0]
    current  = struct.unpack('>i', frame[8:12])[0]
    t = (frame[12] << 16) | (frame[13] << 8) | frame[14]
    hrs, rem = divmod(t, 3600)
    mins, secs = divmod(rem, 60)
    remaining = f"{hrs:02d}:{mins:02d}:{secs:02d}"

    return {
        'soc'      : soc,
        'voltage'  : voltage,
        'capacity' : capacity,
        'current'  : current,
        'remaining': remaining
    }

def main():
    port = find_uart_port()
    if not port:
        print("Fant ingen av kandidat-portene:", ', '.join(CANDIDATE_PORTS))
        sys.exit(1)

    try:
        ser = serial.Serial(port, BAUDRATE, timeout=TIMEOUT)
    except serial.SerialException as e:
        print(f"Kunne ikke åpne port {port}: {e}")
        sys.exit(1)

    print(f"Åpnet {port} @ {BAUDRATE} bps")
    buf = bytearray()

    try:
        while True:
            b = ser.read(1)
            if not b:
                continue

            # synkroniser på header 0xA5
            if not buf:
                if b[0] != 0xA5:
                    continue
                buf.append(b[0])
            else:
                buf.append(b[0])

            # når vi har 16 byte, parse og vis SOC
            if len(buf) == 16:
                frame = bytes(buf)
                data = parse_frame(frame)
                if data:
                    ts = time.strftime('%Y-%m-%d %H:%M:%S')
                    print(f"[{ts}] SOC = {data['soc']}%  |  V = {data['voltage']:.2f}V  |  I = {data['current']}mA  |  RemTime = {data['remaining']}")
                else:
                    print("Ugyldig ramme eller checksum-feil:", frame.hex())
                buf.clear()

    except KeyboardInterrupt:
        print("\nAvslutter lesing.")
    finally:
        ser.close()

if __name__ == '__main__':
    main()
