#!/usr/bin/env python3
import serial
import struct
import time

# ---- KONFIGURASJON ----
PORT = '/dev/ttys0'   # Eller '/dev/ttyAMA0' om du ikke bruker serial0-pekeren
BAUDRATE = 9600
TIMEOUT = 1             # sekunder

# ---- HJELPEFUNKSJONER ----
def parse_frame(frame: bytes):
    """
    Tar en 16-byte frame og returnerer en dict med utpakkede verdier,
    eller None ved format/checksum-feil.
    """
    if len(frame) != 16 or frame[0] != 0xA5:
        return None
    # Sjekk checksum: 8-bit sum av byte 0..14 skal være byte 15
    if (sum(frame[0:15]) & 0xFF) != frame[15]:
        return None

    soc = frame[1]  # i %
    voltage = ((frame[2] << 8) | frame[3]) / 100.0
    capacity = struct.unpack('>I', frame[4:8])[0]
    current = struct.unpack('>i', frame[8:12])[0]
    t = (frame[12] << 16) | (frame[13] << 8) | frame[14]
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
    """
    Leser kontinuerlig fra serielt grensesnitt, synkroniserer
    på 0xA5, pakker ut hele frame og printer resultat.
    """
    buf = bytearray()
    while True:
        # Les én byte
        b = ser.read(1)
        if not b:
            continue

        # Hvis vi ikke har header ennå, let etter 0xA5
        if not buf and b[0] != 0xA5:
            continue

        buf += b

        # Når vi har 16 byte, prøv å parse
        if len(buf) == 16:
            data = parse_frame(buf)
            if data:
                # Skriv ut i terminalen
                print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] "
                      f"SOC={data['SOC (%)']}% | "
                      f"V={data['Voltage (V)']:.2f}V | "
                      f"Cap={data['Capacity (mAh)']}mAh | "
                      f"I={data['Current (mA)']}mA | "
                      f"RemTime={data['Remaining Time']}")
            else:
                print("Ugyldig frame eller checksum-feil")

            # Nullstill buffer og vent på ny header
            buf = bytearray()

if __name__ == '__main__':
    try:
        ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)
        print(f"Åpnet {PORT} @ {BAUDRATE} bps")
        read_loop(ser)
    except serial.SerialException as e:
        print("Kunne ikke åpne seriel port:", e)
    except KeyboardInterrupt:
        print("\nAvslutter lesing.")
