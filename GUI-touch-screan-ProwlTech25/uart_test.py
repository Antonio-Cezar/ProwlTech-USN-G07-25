#!/usr/bin/env python3
import serial
import time
import sys

def åpne_port(port='/dev/ttyS0', baudrate=9600, timeout=1):
    """
    Prøver å åpne serial-porten. Kaster serial.SerialException hvis det feiler.
    """
    try:
        ser = serial.Serial(port=port, baudrate=baudrate, timeout=timeout)
        print(f"Åpnet {port} @ {baudrate} bps")
        return ser
    except serial.SerialException as e:
        print(f"FEIL: Klarte ikke å åpne port {port}: {e}", file=sys.stderr)
        sys.exit(1)

def les_pakke(ser):
    """
    Leser én 16-bytes pakke, synkronisert på header 0xA5.
    Returnerer en dict med feltene eller None ved feil.
    """
    # Finn header 0xA5
    while True:
        b = ser.read(1)
        if not b:
            print("TIMEOUT: Ingen data mottatt på 1 s", file=sys.stderr)
            return None
        if b == b'\xA5':
            break

    rest = ser.read(15)
    if len(rest) != 15:
        print(f"FEIL: Ufullstendig pakke (leste {len(rest)} bytes)", file=sys.stderr)
        return None

    pakke = b'\xA5' + rest

    # Sjekk checksum: 8-bit sum over byte 1–15
    cs_regnet = sum(pakke[:15]) & 0xFF
    cs_mottatt = pakke[15]
    if cs_regnet != cs_mottatt:
        print(f"FEIL: Checksum mismatch (regnet={cs_regnet:02X}, mottatt={cs_mottatt:02X})", file=sys.stderr)
        return None

    # Tolk feltene i henhold til protokollen :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}
    soc      = pakke[1]  # 0–100%
    volt_raw = int.from_bytes(pakke[2:4], byteorder='big')  # i 0.01 V
    voltage  = volt_raw / 100.0
    cap_mAh  = int.from_bytes(pakke[4:8], byteorder='big')  # i mAh
    curr_mA  = int.from_bytes(pakke[8:12], byteorder='big', signed=True)  # i mA
    rem_s    = int.from_bytes(pakke[12:15], byteorder='big')  # i sekunder

    return {
        'soc_percent': soc,
        'voltage_V':   voltage,
        'capacity_mAh': cap_mAh,
        'current_mA':   curr_mA,
        'remain_sec':   rem_s,
    }

def main():
    ser = åpne_port(port='/dev/serial0', baudrate=9600, timeout=1)
    try:
        while True:
            data = les_pakke(ser)
            if data:
                # Formater og vis
                hrs, rem = divmod(data['remain_sec'], 3600)
                mins, secs = divmod(rem, 60)
                print(f"SOC: {data['soc_percent']:3d}%  "
                      f"Volt: {data['voltage_V']:6.2f} V  "
                      f"Kap.: {data['capacity_mAh']:5d} mAh  "
                      f"I: {data['current_mA']:6d} mA  "
                      f"Tid igjen: {int(hrs):02d}:{int(mins):02d}:{int(secs):02d}")
            # Vent litt før neste forsøk (shunten sender 1 gang/s)
            time.sleep(0.1)

    except KeyboardInterrupt:
        print("\nAvslutter…")
    finally:
        ser.close()

if __name__ == '__main__':
    main()
