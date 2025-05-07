#!/usr/bin/env python3
import serial
import time

PORT     = '/dev/ttyS0'   # Mini-UART på GPIO 14/15
BAUDRATE = 9600
TIMEOUT  = 2              # sekunder per readline()

def main():
    try:
        ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)
        print(f"Åpnet {PORT} @ {BAUDRATE} bps")
    except Exception as e:
        print("Kunne ikke åpne port:", e)
        return

    # Gi porten et par sekunder til å komme i gang
    time.sleep(2)

    # Send en “wake up”-kommando (CRLF)
    print("Sender wake-up (CRLF)...")
    ser.write(b'\r\n')

    print("Venter på svar fra shunt (readline)...\n")
    try:
        while True:
            line = ser.readline()           # les inntil '\n'
            if line:
                # Skriv både rått og tolket
                print("RAW LINE:", line)
                print("AS TEXT :", line.decode('utf8', errors='replace').strip())
                print("HEX      ", line.hex())
                print("-" * 30)
            else:
                print("Ingen data... (timeout)")
    except KeyboardInterrupt:
        print("\nAvslutter.")
    finally:
        ser.close()

if __name__ == '__main__':
    main()
