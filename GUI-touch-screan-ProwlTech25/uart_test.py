#!/usr/bin/env python3
import serial, struct, time

SERIAL_PORT = '/dev/ttyS0'   # <-- stor S!
BAUDRATE    = 9600
TIMEOUT     = 1

def parse_frame(f):
    if len(f)!=16 or f[0]!=0xA5: return None
    if (sum(f[:15])&0xFF)!=f[15]: return None
    soc = f[1]
    return {'soc': soc}

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=TIMEOUT)
        print(f"Åpnet {SERIAL_PORT} @ {BAUDRATE}")
    except Exception as e:
        print("Klarte ikke åpne port:", e)
        return

    buf = bytearray()
    try:
        while True:
            b = ser.read(1)
            if not b: 
                continue
            print("DEBUG byte:", b.hex())
            if not buf and b[0]!=0xA5:
                continue
            buf += b
            print("DEBUG len:", len(buf))
            if len(buf)==16:
                print("DEBUG frame:", buf.hex())
                d = parse_frame(buf)
                if d:
                    print(f"SOC = {d['soc']}%")
                else:
                    print("Ugyldig ramme")
                buf.clear()
    except KeyboardInterrupt:
        pass
    finally:
        ser.close()

if __name__=='__main__':
    main()
