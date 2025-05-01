#!/usr/bin/env python3
import serial, struct, time, argparse, sys, glob

DEFAULT_PORTS = ['/dev/serial0', '/dev/ttyAMA0', '/dev/ttyS0']
BAUDRATE = 9600
TIMEOUT = 1  # sekunder

def find_port():
    for p in DEFAULT_PORTS:
        if glob.glob(p):
            return p
    return None

def parse_args():
    p = argparse.ArgumentParser(description="Les data fra batterishunt over TTL-serial")
    p.add_argument('-p', '--port', help="Serial port (default: prøver serial0, ttyAMA0, ttyS0)")
    return p.parse_args()

def parse_frame(frame: bytes):
    if len(frame) != 16 or frame[0] != 0xA5:
        return None
    if (sum(frame[0:15]) & 0xFF) != frame[15]:
        return None

    soc = frame[1]
    voltage = ((frame[2]<<8)|frame[3]) / 100.0
    capacity = struct.unpack('>I', frame[4:8])[0]
    current = struct.unpack('>i', frame[8:12])[0]
    t = (frame[12]<<16)|(frame[13]<<8)|frame[14]
    hrs, rem = divmod(t, 3600)
    mins, secs = divmod(rem, 60)

    return {
        'SOC (%)': soc,
        'Voltage (V)': voltage,
        'Capacity (mAh)': capacity,
        'Current (mA)': current,
        'Remaining Time': f"{hrs:02d}:{mins:02d}:{secs:02d}"
    }

def read_loop(ser):
    buf = bytearray()
    while True:
        b = ser.read(1)
        if not b:
            continue
        if not buf and b[0] != 0xA5:
            continue
        buf += b
        if len(buf) == 16:
            data = parse_frame(buf)
            if data:
                print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] "
                      f"SOC={data['SOC (%)']}% | "
                      f"V={data['Voltage (V)']:.2f}V | "
                      f"Cap={data['Capacity (mAh)']}mAh | "
                      f"I={data['Current (mA)']}mA | "
                      f"RemTime={data['Remaining Time']}")
            else:
                print("Ugyldig frame eller checksum-feil")
            buf.clear()

if __name__ == '__main__':
    args = parse_args()
    port = args.port or find_port()
    if not port:
        print("Fant ingen serial-port. Forsøk å aktivere i raspi-config eller spesifiser -p")
        sys.exit(1)

    try:
        ser = serial.Serial(port, BAUDRATE, timeout=TIMEOUT)
        print(f"Åpnet {port} @ {BAUDRATE} bps")
        read_loop(ser)
    except serial.SerialException as e:
        print("Kunne ikke åpne seriel port:", e)
    except KeyboardInterrupt:
        print("\nAvslutter.")
